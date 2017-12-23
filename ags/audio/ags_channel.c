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

#include <ags/audio/ags_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/client/ags_remote_channel.h>

#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_recycling_thread.h>

#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_remove_recall.h>
#include <ags/audio/task/ags_remove_recall_container.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <stdlib.h>
#include <stdio.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

#include <ags/i18n.h>

/**
 * SECTION:ags_channel
 * @short_description: Acts as entry point to the audio tree.
 * @title: AgsChannel
 * @section_id:
 * @include: ags/audio/ags_channel.h
 *
 * #AgsChannel is the entry point to the entire audio tree and its nested
 * recycling tree.
 *
 * Every channel has its own #AgsRecallID. As modifying link a new #AgsRecyclingContext
 * is indicated, since it acts as a kind of recall id tree context.
 */

void ags_channel_class_init(AgsChannelClass *channel_class);
void ags_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_init(AgsChannel *channel);
void ags_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_channel_add_to_registry(AgsConnectable *connectable);
void ags_channel_remove_from_registry(AgsConnectable *connectable);
void ags_channel_connect(AgsConnectable *connectable);
void ags_channel_disconnect(AgsConnectable *connectable);
void ags_channel_dispose(GObject *gobject);
void ags_channel_finalize(GObject *gobject);

GList* ags_channel_add_ladspa_effect(AgsChannel *channel,
				     gchar *filename,
				     gchar *effect);
GList* ags_channel_add_dssi_effect(AgsChannel *channel,
				   gchar *filename,
				   gchar *effect);
GList* ags_channel_add_lv2_effect(AgsChannel *channel,
				  gchar *filename,
				  gchar *effect);
GList* ags_channel_real_add_effect(AgsChannel *channel,
				   gchar *filename,
				   gchar *effect);
void ags_channel_real_remove_effect(AgsChannel *channel,
				    guint nth);
void ags_channel_real_done(AgsChannel *channel,
			   AgsRecallID *recall_id);

enum{
  ADD_EFFECT,
  REMOVE_EFFECT,
  RECYCLING_CHANGED,
  DONE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SOUNDCARD,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_PAD,
  PROP_AUDIO_CHANNEL,
  PROP_LINE,
  PROP_NOTE,
  PROP_REMOTE_CHANNEL,
  PROP_PLAYBACK,
  PROP_RECALL_ID,
  PROP_RECALL_CONTAINER,
  PROP_RECALL,
  PROP_PLAY,
  PROP_LINK,
  PROP_FIRST_RECYCLING,
  PROP_LAST_RECYCLING,
  PROP_PATTERN,
};

static gpointer ags_channel_parent_class = NULL;
static guint channel_signals[LAST_SIGNAL];

GType
ags_channel_get_type (void)
{
  static GType ags_type_channel = 0;

  if(!ags_type_channel){
    static const GTypeInfo ags_channel_info = {
      sizeof (AgsChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel = g_type_register_static(G_TYPE_OBJECT,
					      "AgsChannel",
					      &ags_channel_info, 0);

    g_type_add_interface_static(ags_type_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_channel);
}

void
ags_channel_class_init(AgsChannelClass *channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_channel_parent_class = g_type_class_peek_parent(channel);

  /* GObjectClass */
  gobject = (GObjectClass *) channel;

  gobject->set_property = ags_channel_set_property;
  gobject->get_property = ags_channel_get_property;

  gobject->dispose = ags_channel_dispose;
  gobject->finalize = ags_channel_finalize;

  /* properties */
  /**
   * AgsChannel:audio:
   *
   * The assigned #AgsAudio aligning channels.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsChannel:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudio:samplerate:
   *
   * The samplerate.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate"),
				  i18n_pspec("The samplerate"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudio:buffer-size:
   *
   * The buffer size.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size"),
				  i18n_pspec("The buffer size"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudio:format:
   *
   * The format.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format"),
				  i18n_pspec("The format"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);
  
  /**
   * AgsAudio:audio-channel:
   *
   * The nth audio channel.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("nth audio channel"),
				  i18n_pspec("The nth audio channel"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsAudio:pad:
   *
   * The nth pad.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("pad",
				  i18n_pspec("nth pad"),
				  i18n_pspec("The nth pad"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsAudio:line:
   *
   * The nth line.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_uint("line",
				  i18n_pspec("nth line"),
				  i18n_pspec("The nth line"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsChannel:note:
   *
   * The assigned #AgsNote representing this channel.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("note",
				   i18n_pspec("assigned note"),
				   i18n_pspec("The note it is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /**
   * AgsChannel:playback:
   *
   * The assigned #AgsPlayback.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("assigned playback"),
				   i18n_pspec("The playback it is assigned with"),
				   AGS_TYPE_PLAYBACK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);

  /**
   * AgsChannel:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("recall-id",
				    i18n_pspec("assigned recall id"),
				    i18n_pspec("The recall id it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsChannel:recall-container:
   *
   * The containing #AgsRecallContainer.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("recall-container",
				    i18n_pspec("containing recall-container"),
				    i18n_pspec("The recall container it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsChannel:recall:
   *
   * The containing #AgsRecall in recall-context.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("recall",
				    i18n_pspec("containing recall"),
				    i18n_pspec("The recall it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);

  /**
   * AgsChannel:play:
   *
   * The containing #AgsRecall in play-context.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("play",
				    i18n_pspec("containing play"),
				    i18n_pspec("The play it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /**
   * AgsChannel:link:
   *
   * The assigned link as #AgsChannel.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("link",
				   i18n_pspec("assigned link"),
				   i18n_pspec("The link it is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINK,
				  param_spec);

  /**
   * AgsChannel:first-recycling:
   *
   * The containing #AgsRecycling it takes it #AgsAudioSignal from.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("first-recycling",
				   i18n_pspec("containing first recycling"),
				   i18n_pspec("The first recycling it contains"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_RECYCLING,
				  param_spec);

  /**
   * AgsChannel:last-recycling:
   *
   * The containing #AgsRecycling it takes it #AgsAudioSignal from.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("last-recycling",
				   i18n_pspec("containing last recycling"),
				   i18n_pspec("The last recycling it contains"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_LAST_RECYCLING,
				  param_spec);

  /**
   * AgsChannel:pattern:
   *
   * The containing #AgsPattern.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("pattern",
				    i18n_pspec("containing pattern"),
				    i18n_pspec("The pattern it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);
  
  /* AgsChannelClass */
  channel->add_effect = ags_channel_real_add_effect;
  channel->remove_effect = ags_channel_real_remove_effect;

  channel->recycling_changed = NULL;

  channel->done = ags_channel_real_done;

  /* signals */
    /**
   * AgsChannel::add-effect:
   * @channel: the #AgsChannel to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   */
  channel_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__STRING_STRING,
		 G_TYPE_POINTER, 2,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsChannel::remove-effect:
   * @channel: the #AgsChannel to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   */
  channel_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsChannelClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsChannel::recycling-changed:
   * @channel the object recycling changed
   * @old_start_region: first recycling
   * @old_end_region: last recycling
   * @new_start_region: new first recycling
   * @new_end_region: new last recycling
   * @old_start_changed_region: modified link recycling start
   * @old_end_changed_region: modified link recyclig end
   * @new_start_changed_region: replacing link recycling start
   * @new_end_changed_region: replacing link recycling end
   *
   * The ::recycling-changed signal is invoked to notify modified recycling tree.
   */
  channel_signals[RECYCLING_CHANGED] =
    g_signal_new("recycling-changed",
		 G_TYPE_FROM_CLASS (channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsChannelClass, recycling_changed),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT,
		 G_TYPE_NONE, 8,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT);
  /**
   * AgsChannel::done:
   * @channel: the object done playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::done signal is invoked during termination of playback.
   */
  channel_signals[DONE] =
    g_signal_new("done",
		 G_TYPE_FROM_CLASS (channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsChannelClass, done),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_channel_add_to_registry;
  connectable->remove_from_registry = ags_channel_remove_from_registry;

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_channel_connect;
  connectable->disconnect = ags_channel_disconnect;
}

GQuark
ags_channel_error_quark()
{
  return(g_quark_from_static_string("ags-channel-error-quark"));
}

void
ags_channel_init(AgsChannel *channel)
{
  AgsConfig *config;
  AgsMutexManager *mutex_manager;

  gchar *str;
  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* create mutex */
  channel->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  channel->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* insert mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) channel,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* config */
  config = ags_config_get_instance();

  /* base init */
  channel->flags = 0;

  channel->audio = NULL;
  channel->soundcard = NULL;
    
  channel->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  channel->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  channel->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    channel->samplerate = g_ascii_strtoull(str,
					   NULL,
					   10);

    free(str);
  }else{
    channel->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    channel->buffer_size = g_ascii_strtoull(str,
					    NULL,
					    10);

    free(str);
  }else{
    channel->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    channel->format = g_ascii_strtoull(str,
				       NULL,
				       10);

    free(str);
  }else{
    channel->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  }

  /* inter-connected channels */
  channel->prev = NULL;
  channel->prev_pad = NULL;
  channel->next = NULL;
  channel->next_pad = NULL;

  /* allocation info */
  channel->pad = 0;
  channel->audio_channel = 0;
  channel->line = 0;

  channel->note = NULL;

  /* playback */
  channel->playback = (GObject *) ags_playback_new();
  g_object_ref(channel->playback);
  g_object_set(channel->playback,
	       "source", channel,
	       NULL);

  channel->recall_id = NULL;

  /* recall */
  channel->recall_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  channel->recall_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel->recall_mutex,
		     attr);

  channel->play_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  channel->play_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel->play_mutex,
		     attr);

  channel->container = NULL;
  channel->recall = NULL;
  channel->play = NULL;

  /* link and recycling */
  channel->link = NULL;
  
  channel->first_recycling = NULL;
  channel->last_recycling = NULL;

  /* pattern */
  channel->pattern = NULL;

  /* data */
  channel->line_widget = NULL;
  channel->file_data = NULL;
}

void
ags_channel_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;
  
  channel = AGS_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if((AgsAudio *) channel->audio == audio){
	return;
      }

      if(channel->audio != NULL){
	g_object_unref(G_OBJECT(channel->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      channel->audio = (GObject *) audio;
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_channel_set_soundcard(channel, (GObject *) soundcard);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_channel_set_samplerate(channel,
				 samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_channel_set_buffer_size(channel,
				  buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      ags_channel_set_format(channel,
			     format);
    }
    break;
  case PROP_NOTE:
    {
      gchar *note;

      note = g_value_get_string(value);

      if(channel->note == note){
	return;
      }

      if(channel->note != NULL){
	g_free(channel->note);
      }

      channel->note = g_strdup(channel->note);
    }
    break;
  case PROP_PLAYBACK:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_object(value);

      if(channel->playback == (GObject *) playback){
	return;
      }

      if(channel->playback != NULL){
	g_object_unref(channel->playback);
      }

      if(playback != NULL){
	g_object_ref(playback);
      }

      channel->playback = (GObject *) playback;
    }
    break;
  case PROP_REMOTE_CHANNEL:
    {
      AgsRemoteChannel *remote_channel;

      remote_channel = (AgsRemoteChannel *) g_value_get_object(value);

      if(remote_channel == NULL ||
	 g_list_find(channel->remote_channel, remote_channel) != NULL){
	return;
      }

      ags_channel_add_remote_channel(channel,
				     (GObject *) remote_channel);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_pointer(value);

      if(recall_id == NULL ||
	 g_list_find(channel->recall_id, recall_id) != NULL){
	return;
      }

      ags_channel_add_recall_id(channel,
				recall_id);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_pointer(value);

      if(recall_container == NULL ||
	 g_list_find(channel->container, recall_container) != NULL){
	return;
      }

      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
    }
    break;
  case PROP_RECALL:
    {
      AgsRecall *recall;

      gboolean recall_added;
      
      recall = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(channel->recall_mutex);

      recall_added = (g_list_find(channel->recall, recall) != NULL) ? TRUE: FALSE;
      
      pthread_mutex_unlock(channel->recall_mutex);

      if(recall == NULL ||
	 recall_added){
	return;
      }

      ags_channel_add_recall(channel,
			     (GObject *) recall,
			     FALSE);
    }
    break;
  case PROP_PLAY:
    {
      AgsRecall *play;

      gboolean play_added;
      
      play = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(channel->play_mutex);

      play_added = (g_list_find(channel->play, play) != NULL) ? TRUE: FALSE;
      
      pthread_mutex_unlock(channel->play_mutex);
	    
      if(play == NULL ||
	 play_added){
	return;
      }

      ags_channel_add_recall(channel,
			     (GObject *) play,
			     TRUE);
    }
    break;
  case PROP_LINK:
    {
      AgsChannel *link;

      link = (AgsChannel *) g_value_get_object(value);

      if(channel->link == link){
	return;
      }

      ags_channel_set_link(channel,
			   link,
			   NULL);
    }
    break;
  case PROP_PATTERN:
    {
      AgsPattern *pattern;

      pattern = (AgsPattern *) g_value_get_pointer(value);

      if(pattern == NULL ||
	 g_list_find(channel->pattern, pattern) != NULL){
	return;
      }

      ags_channel_add_pattern(channel,
			      (GObject *) pattern);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;

  channel = AGS_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, channel->audio);
    }
    break;
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, channel->soundcard);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, channel->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, channel->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, channel->format);
    }
    break;
  case PROP_PAD:
    {
      g_value_set_uint(value, channel->pad);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, channel->audio_channel);
    }
    break;
  case PROP_LINE:
    {
      g_value_set_uint(value, channel->line);
    }
    break;
  case PROP_NOTE:
    {
      g_value_set_string(value, channel->note);
    }
    break;
  case PROP_REMOTE_CHANNEL:
    {
      g_value_set_pointer(value, g_list_copy(channel->remote_channel));
    }
    break;
  case PROP_PLAYBACK:
    {
      g_value_set_object(value, channel->playback);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_pointer(value, g_list_copy(channel->recall_id));
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_value_set_pointer(value, g_list_copy(channel->container));
    }
    break;
  case PROP_RECALL:
    {
      pthread_mutex_lock(channel->recall_mutex);
      
      g_value_set_pointer(value, g_list_copy(channel->recall));

      pthread_mutex_unlock(channel->recall_mutex);
    }
    break;
  case PROP_PLAY:
    {
      pthread_mutex_lock(channel->play_mutex);
      
      g_value_set_pointer(value, g_list_copy(channel->play));
      
      pthread_mutex_unlock(channel->play_mutex);      
    }
    break;
  case PROP_LINK:
    {
      g_value_set_object(value, channel->link);
    }
    break;
  case PROP_FIRST_RECYCLING:
    {
      g_value_set_object(value, channel->first_recycling);
    }
    break;
  case PROP_LAST_RECYCLING:
    {
      g_value_set_object(value, channel->last_recycling);
    }
    break;
  case PROP_PATTERN:
    {
      g_value_set_pointer(value, g_list_copy(channel->pattern));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_add_to_registry(AgsConnectable *connectable)
{
  AgsChannel *channel;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;
  
  channel = AGS_CHANNEL(connectable);

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(channel->soundcard));

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) channel);
    ags_registry_add_entry(registry,
			   entry);
  }
  
  /* add play */
  list = channel->play;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
  
  /* add recall */
  list = channel->recall;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_channel_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_channel_connect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsRecycling *recycling;

  GList *list;
  
  channel = AGS_CHANNEL(connectable);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    return;
  }

  channel->flags |= AGS_CHANNEL_CONNECTED;
  
#ifdef AGS_DEBUG
  g_message("connecting channel");
#endif

  //  ags_connectable_add_to_registry(connectable);

  /* connect recall ids */
  list = channel->recall_id;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = channel->container;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = channel->recall;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = channel->play;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    while(recycling != channel->last_recycling->next){
      ags_connectable_connect(AGS_CONNECTABLE(recycling));
      
      recycling = recycling->next;
    }
  }

  /* connect pattern and notation */
  list = channel->pattern;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_channel_disconnect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsRecycling *recycling;

  GList *list;
  
  channel = AGS_CHANNEL(connectable);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) == 0){
    return;
  }
  
  channel->flags &= (~AGS_CHANNEL_CONNECTED);
  
#ifdef AGS_DEBUG
  g_message("disconnecting channel");
#endif

  //  ags_connectable_add_to_registry(connectable);

  /* connect recall ids */
  list = channel->recall_id;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = channel->container;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = channel->recall;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = channel->play;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    while(recycling != channel->last_recycling->next){
      ags_connectable_disconnect(AGS_CONNECTABLE(recycling));
      
      recycling = recycling->next;
    }
  }

  /* connect pattern and notation */
  list = channel->pattern;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_channel_dispose(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;
  AgsRecycling *end_region;

  GList *list, *list_next;
  
  gboolean dispose_recycling;
  
  channel = AGS_CHANNEL(gobject);

  /* audio */
  dispose_recycling = FALSE;

  if(channel->audio != NULL){
    if(((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 &&
	AGS_IS_INPUT(channel) &&
	channel->link == NULL) ||
       ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 &&
	AGS_IS_OUTPUT(channel))){
      dispose_recycling = TRUE;
    }
    
    g_object_unref(channel->audio);

    channel->audio = NULL;
  }

  /* soundcard */
  if(channel->soundcard != NULL){
    g_object_unref(channel->soundcard);

    channel->soundcard = NULL;
  }

  /* recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    end_region = channel->last_recycling->next;
    
    while(recycling != end_region){
      recycling_next = recycling->next;

      if(dispose_recycling){
	g_object_run_dispose(recycling);
	g_object_unref((GObject *) recycling);
      }
      
      recycling = recycling_next;
    }

    channel->first_recycling = NULL;
    channel->last_recycling = NULL;
  }

  /* remote channel */
  if(channel->remote_channel != NULL){
    list = channel->remote_channel;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->remote_channel,
		     g_object_unref);
  
    channel->remote_channel = NULL;
  }
  
  /* playback */
  if(channel->playback != NULL){
    AgsPlayback *playback;

    playback = channel->playback;
    
    ags_thread_stop(playback->channel_thread[0]);
    ags_thread_stop(playback->channel_thread[1]);
    ags_thread_stop(playback->channel_thread[2]);
    
    g_object_set(playback,
		 "source", NULL,
		 NULL);
    //    g_object_run_dispose(playback);
    g_object_unref(playback);
  
    channel->playback = NULL;
  }

  /* recall id */
  if(channel->recall_id != NULL){
    list = channel->recall_id;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->recall_id,
		     g_object_unref);
  
    channel->recall_id = NULL;
  }
  
  /* recall container */
  if(channel->container != NULL){
    list = channel->container;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->container,
		     g_object_unref);

    channel->container = NULL;
  }
  
  /* recall */
  if(channel->recall != NULL){
    pthread_mutex_lock(channel->recall_mutex);

    list = channel->recall;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->recall,
		     g_object_unref);
  
    channel->recall = NULL;

    pthread_mutex_unlock(channel->recall_mutex);
  }
  
  /* play */
  if(channel->play != NULL){
    pthread_mutex_lock(channel->play_mutex);

    list = channel->play;

    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(channel->play,
		     g_object_unref);

    channel->play = NULL;

    pthread_mutex_unlock(channel->play_mutex);
  }
  
  /* pattern */
  if(channel->remote_channel != NULL){
    list = channel->pattern;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }

    
    g_list_free_full(channel->pattern,
		     g_object_unref);

    channel->pattern = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_channel_parent_class)->dispose(gobject);
}

void
ags_channel_finalize(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  channel = AGS_CHANNEL(gobject);

  pthread_mutexattr_destroy(channel->obj_mutexattr);
  free(channel->obj_mutexattr);

  /* audio */
  if(channel->audio != NULL){
    g_object_unref(channel->audio);
  }

  /* soundcard */
  if(channel->soundcard != NULL){
    g_object_unref(channel->soundcard);
  }

  /* recycling */
  recycling = channel->first_recycling;

  if(recycling != NULL){
    while(recycling != channel->last_recycling->next){
      recycling_next = recycling->next;

      g_object_unref((GObject *) recycling);

      recycling = recycling_next;
    }
  }

  /* key string */
  if(channel->note != NULL){
    free(channel->note);
  }

  /* remote channel */
  if(channel->remote_channel != NULL){
    g_list_free_full(channel->remote_channel,
		     g_object_unref);
  }

  /* playback */
  if(channel->playback != NULL){
    g_object_unref(channel->playback);
  }

  /* recall id */
  g_list_free_full(channel->recall_id,
		   g_object_unref);

  /* recall */
  g_list_free_full(channel->container,
		   g_object_unref);
  
  g_list_free_full(channel->recall,
		   g_object_unref);
  g_list_free_full(channel->play,
		   g_object_unref);

  pthread_mutex_destroy(channel->recall_mutex);
  free(channel->recall_mutex);

  pthread_mutexattr_destroy(channel->recall_mutexattr);
  free(channel->recall_mutexattr);

  pthread_mutex_destroy(channel->play_mutex);
  free(channel->play_mutex);

  pthread_mutexattr_destroy(channel->play_mutexattr);
  free(channel->play_mutexattr);

  free(channel->play_mutex);
  
  /* pattern */
  g_list_free_full(channel->pattern,
		   g_object_unref);
  
  /* call parent class */
  G_OBJECT_CLASS(ags_channel_parent_class)->finalize(gobject);
}

AgsRecall*
ags_channel_find_recall(AgsChannel *channel, char *effect, char *name)
{
  AgsRecall *recall;
  GList *list;

  //TODO:JK: implement me
  /* */

  return(NULL);
}

/**
 * ags_channel_first:
 * @channel: an #AgsChannel
 *
 * Iterates until the first #AgsChannel was found.
 *
 * Returns: the first #AgsChannel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_first(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  /* first */
  channel = ags_channel_pad_first(channel);
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* iterate */
  while(channel != NULL){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(mutex);

    if(channel->prev == NULL){
      pthread_mutex_unlock(mutex);

      break;
    }else{
      channel = channel->prev;
      
      pthread_mutex_unlock(mutex);
    }
  }
  
  return(channel);
}

/**
 * ags_channel_last:
 * @channel: an #AgsChannel
 *
 * Iterates until the last #AgsChannel was found.
 *
 * Returns: the last #AgsChannel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_last(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }
  
  /* last */
  channel = ags_channel_pad_last(channel);

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* iterate */
  while(channel != NULL){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(mutex);

    if(channel->next == NULL){    
      pthread_mutex_unlock(mutex);

      break;
    }else{
      channel = channel->next;
    
      pthread_mutex_unlock(mutex);
    }
  }

  return(channel);
}

/**
 * ags_channel_nth:
 * @channel: an #AgsChannel
 * @nth: the count to iterate
 *
 * Iterates @nth times forward.
 *
 * Returns: the nth #AgsChannel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_nth(AgsChannel *channel, guint nth)
{
  AgsMutexManager *mutex_manager;

  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* nth */
  for(i = 0; i < nth && channel != NULL; i++){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);
  
    pthread_mutex_lock(mutex);
    
    channel = channel->next;
    
    pthread_mutex_unlock(mutex);
  }

#ifdef AGS_DEBUG
  if((nth != 0 && i != nth) || channel == NULL){
    g_message("ags_channel_nth:\n  nth channel does not exist\n  `- stopped @: i = %u; nth = %u\n", i, nth);
  }
#endif
  
  return(channel);
}

/**
 * ags_channel_pad_first:
 * @channel: an #AgsChannel
 *
 * Iterates until the first pad has been reached.
 *
 * Returns: the first #AgsChannel with the same audio_channel as @channel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_pad_first(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lookup mutex */
  pthread_mutex_lock(application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  
  pthread_mutex_unlock(application_mutex);

  /* pad first */
  while(channel != NULL){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(mutex);

    if(channel->prev_pad == NULL){
      pthread_mutex_unlock(mutex);

      break;
    }else{
      channel = channel->prev_pad;
      
      pthread_mutex_unlock(mutex);
    }
  }

  return(channel);
}

/**
 * ags_channel_pad_last:
 * @channel: an #AgsChannel
 *
 * Iterates until the last pad has been reached.
 *
 * Returns: the last #AgsChannel with the same audio_channel as @channel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_pad_last(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  while(channel != NULL){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(mutex);
    
    if(channel->next_pad == NULL){
      pthread_mutex_unlock(mutex);
      
      break;
    }else{
      channel = channel->next_pad;

      pthread_mutex_unlock(mutex);
    }
  }

  return(channel);
}

/**
 * ags_channel_pad_nth:
 * @channel: an #AgsChannel
 * @nth: the count of pads to step
 *
 * Iterates nth times.
 *
 * Returns: the nth pad
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_pad_nth(AgsChannel *channel, guint nth)
{
  AgsMutexManager *mutex_manager;

  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* pad nth */
  for(i = 0; i < nth && channel != NULL; i++){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);
  
    pthread_mutex_lock(mutex);

    channel = channel->next_pad;
    
    pthread_mutex_unlock(mutex);
  }

#ifdef AGS_DEBUG
  if(channel == NULL){
    g_message("ags_channel_nth_pad:\n  nth pad does not exist\n  `- stopped @: i = %u; nth = %u", i, nth);
  }
#endif
  
  return(channel);
}

/**
 * ags_channel_first_with_recycling:
 * @channel: an #AgsChannel
 *
 * Iterates over pads forwards as long as there is no #AgsRecycling on the #AgsChannel.
 *
 * Returns: the first channel with an #AgsRecycling
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_first_with_recycling(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  channel = ags_channel_pad_first(channel);

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* first with recycling */
  while(channel != NULL){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);
  
    pthread_mutex_lock(mutex);
    
    if(channel->first_recycling != NULL){
      pthread_mutex_unlock(mutex);
      
      break;
    }

    channel = channel->next_pad;

    pthread_mutex_unlock(mutex);
  }

  return(channel);
}

/**
 * ags_channel_last_with_recycling:
 * @channel: an #AgsChannel
 *
 * Iterates over pads backwards as long as there is no #AgsRecycling on the #AgsChannel.
 *
 * Returns: the last channel with an #AgsRecycling
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_last_with_recycling(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  channel = ags_channel_pad_last(channel);

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* last with recycling */
  while(channel != NULL){
    pthread_mutex_lock(application_mutex);
	
    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);
  
    pthread_mutex_lock(mutex);

    if(channel->last_recycling != NULL){
      pthread_mutex_unlock(mutex);
      break;
    }

    channel = channel->prev_pad;
    
    pthread_mutex_unlock(mutex);
  }

  return(channel);
}

AgsChannel*
ags_channel_prev_with_recycling(AgsChannel *channel)
{
  AgsChannel *current;
  AgsRecycling *recycling;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* prev with recycling */
  pthread_mutex_lock(mutex);
  
  current = channel->prev_pad;
    
  pthread_mutex_unlock(mutex);

  while(current != NULL && current != channel){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) current);
  
    pthread_mutex_unlock(application_mutex);
    
    pthread_mutex_lock(mutex);

    recycling = current->last_recycling;

    pthread_mutex_unlock(mutex);
    
    if(recycling != NULL){
      break;
    }
    
    pthread_mutex_lock(mutex);

    current = current->prev_pad;
    
    pthread_mutex_unlock(mutex);
  }
  
  if(current == channel){
    return(NULL);
  }else{
    return(current);
  }
}
  
AgsChannel*
ags_channel_next_with_recycling(AgsChannel *channel)
{
  AgsChannel *current;
  AgsRecycling *recycling;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* next with recycling */
  pthread_mutex_lock(mutex);

  current = channel->next_pad;

  pthread_mutex_unlock(mutex);

  while(current != NULL && current != channel){
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     (GObject *) current);
  
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(mutex);
    
    recycling = current->first_recycling;
    
    pthread_mutex_unlock(mutex);
    
    if(recycling != NULL){
      break;
    }
    
    pthread_mutex_lock(mutex);
    
    current = current->next_pad;
    
    pthread_mutex_unlock(mutex);
  }  

  if(current == channel){
    return(NULL);
  }else{
    return(current);
  }
}

/**
 * ags_channel_set_soundcard:
 * @channel: an #AgsChannel
 * @soundcard: an #GObject
 *
 * Sets soundcard.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_soundcard(AgsChannel *channel, GObject *soundcard)
{
  AgsMutexManager *mutex_manager;

  GList *list;
  
  guint samplerate;
  guint buffer_size;
  guint format;  

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return;
  }
  
  /* channel */
  if(channel->soundcard == soundcard){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* apply new soundcard */
  pthread_mutex_lock(mutex);

  if(channel->soundcard != NULL){
    g_object_unref(channel->soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  channel->soundcard = (GObject *) soundcard;

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    &format);
  g_object_set(channel,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);

  /* playback */
  if(AGS_PLAYBACK(channel->playback)->channel_thread[0] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[0],
		 "soundcard", soundcard,
		 NULL);
  }
    
  if(AGS_PLAYBACK(channel->playback)->channel_thread[1] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[1],
		 "soundcard", soundcard,
		 NULL);
  }
  
  if(AGS_PLAYBACK(channel->playback)->channel_thread[2] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[2],
		 "soundcard", soundcard,
		 NULL);
  }
  
  /* recall */
  list = channel->play;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard", soundcard,
		 NULL);

    list = list->next;
  }

  list = channel->recall;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard", soundcard,
		 NULL);

    list = list->next;
  }

  /* AgsRecycling */
  if((AGS_IS_OUTPUT(channel) &&
      (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0) ||
     ((AGS_IS_INPUT(channel) &&
       (AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0))){
    AgsRecycling *recycling;

    recycling = channel->first_recycling;

    if(recycling != NULL){
      while(recycling != channel->last_recycling->next){
	g_object_set(G_OBJECT(recycling),
		     "soundcard", soundcard,
		     NULL); 

	recycling = recycling->next;
      }
    }
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_set_samplerate:
 * @audio: the #AgsAudio
 * @samplerate: the samplerate
 *
 * Sets buffer length.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_samplerate(AgsChannel *channel, guint samplerate)
{
  AgsAudio *audio;

  AgsConfig *config;
  
  gchar *str0, *str1;

  gdouble freq;

  auto void ags_channel_set_samplerate_audio_signal(GList *audio_signal, guint samplerate);

  void ags_channel_set_samplerate_audio_signal(GList *audio_signal, guint samplerate){
    AgsAudioSignal *template;
    
    template = ags_audio_signal_get_template(audio_signal);
    
    if(template != NULL){
      ags_audio_signal_set_samplerate(template,
				      samplerate);
    }
  }

  audio = (AgsAudio *) channel->audio;

  channel->samplerate = samplerate;

  /* reset threads */
  config = ags_config_get_instance();
  
  freq = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope");

  if(str0 != NULL && str1 != NULL){
    if(!g_ascii_strncasecmp(str0,
			    "super-threaded",
			    15)){
      if(!g_ascii_strncasecmp(str1,
			      "channel",
			      8) ||
	 !g_ascii_strncasecmp(str1,
			      "recycling",
			      10)){
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[0],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[1],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[2],
		     "frequency", freq,
		     NULL);
      }
    }
  }

  g_free(str0);
  g_free(str1);

  /* reset recycling */
  if(audio != NULL){
    AgsRecycling *recycling;

    GList *audio_signal;
    
    if(AGS_IS_OUTPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(recycling != NULL){
	ags_channel_set_samplerate_audio_signal(recycling->audio_signal,
						samplerate);
      }
    }

    if(AGS_IS_INPUT(channel) &&
       channel->link == NULL &&
       (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(AGS_INPUT(channel)->file_link == NULL){
	if(recycling != NULL){
	  ags_channel_set_samplerate_audio_signal(recycling->audio_signal,
						  samplerate);
	}
      }else{
	AgsFileLink *file_link;

	file_link = (AgsFileLink *) AGS_INPUT(channel)->file_link;
	
	ags_input_open_file(AGS_INPUT(channel),
			    file_link->filename,
			    AGS_AUDIO_FILE_LINK(file_link)->preset,
			    AGS_AUDIO_FILE_LINK(file_link)->instrument,
			    AGS_AUDIO_FILE_LINK(file_link)->sample,
			    AGS_AUDIO_FILE_LINK(file_link)->audio_channel);
      }
    }
  }
}

/**
 * ags_audio_set_buffer_size:
 * @audio: the #AgsAudio
 * @buffer_size: the buffer_size
 *
 * Sets buffer length.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_buffer_size(AgsChannel *channel, guint buffer_size)
{
  AgsAudio *audio;

  AgsConfig *config;
  
  gchar *str0, *str1;

  gdouble freq;

  auto void ags_channel_set_buffer_size_audio_signal(GList *audio_signal, guint buffer_size);

  void ags_channel_set_buffer_size_audio_signal(GList *audio_signal, guint buffer_size){
    AgsAudioSignal *template;
    
    template = ags_audio_signal_get_template(audio_signal);
    
    if(template != NULL){
      ags_audio_signal_set_buffer_size(template,
				       buffer_size);
    }
  }  
  
  audio = (AgsAudio *) channel->audio;
  
  channel->buffer_size = buffer_size;

  /* reset threads */
  config = ags_config_get_instance();
  
  freq = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope");

  if(str0 != NULL && str1 != NULL){
    if(!g_ascii_strncasecmp(str0,
			    "super-threaded",
			    15)){
      if(!g_ascii_strncasecmp(str1,
			      "channel",
			      8) ||
	 !g_ascii_strncasecmp(str1,
			      "recycling",
			      10)){
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[0],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[1],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[2],
		     "frequency", freq,
		     NULL);
      }
    }
  }

  g_free(str0);
  g_free(str1);

  /* reset recycling */
  if(audio != NULL){
    AgsRecycling *recycling;

    GList *audio_signal;
    
    if(AGS_IS_OUTPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(recycling != NULL){
	ags_channel_set_buffer_size_audio_signal(recycling->audio_signal,
						 buffer_size);
      }
    }

    if(AGS_IS_INPUT(channel) &&
       channel->link == NULL &&
       (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(AGS_INPUT(channel)->file_link == NULL){
	if(recycling != NULL){
	  ags_channel_set_buffer_size_audio_signal(recycling->audio_signal,
						   buffer_size);
	}
      }else{
	AgsFileLink *file_link;

	file_link = (AgsFileLink *) AGS_INPUT(channel)->file_link;
	
	ags_input_open_file(AGS_INPUT(channel),
			    file_link->filename,
			    AGS_AUDIO_FILE_LINK(file_link)->preset,
			    AGS_AUDIO_FILE_LINK(file_link)->instrument,
			    AGS_AUDIO_FILE_LINK(file_link)->sample,
			    AGS_AUDIO_FILE_LINK(file_link)->audio_channel);
      }
    }
  }
}

/**
 * ags_audio_set_format:
 * @audio: the #AgsAudio
 * @format: the format
 *
 * Sets buffer length.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_format(AgsChannel *channel, guint format)
{
  AgsAudio *audio;

  auto void ags_channel_set_format_audio_signal(GList *audio_signal, guint format);

  void ags_channel_set_format_audio_signal(GList *audio_signal, guint format){
    AgsAudioSignal *template;
    
    template = ags_audio_signal_get_template(audio_signal);
    
    if(template != NULL){
      ags_audio_signal_set_format(template,
				  format);
    }
  }
  
  audio = (AgsAudio *) channel->audio;
  
  channel->format = format;

  if(audio != NULL){
    AgsRecycling *recycling;

    GList *audio_signal;
    
    if(AGS_IS_OUTPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(recycling != NULL){
	ags_channel_set_format_audio_signal(recycling->audio_signal,
					    format);
      }
    }

    if(AGS_IS_INPUT(channel) &&
       channel->link == NULL &&
       (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      recycling = channel->first_recycling;

      if(AGS_INPUT(channel)->file_link == NULL){
	if(recycling != NULL){
	  ags_channel_set_format_audio_signal(recycling->audio_signal,
					      format);
	}
      }else{
	AgsFileLink *file_link;

	file_link = (AgsFileLink *) AGS_INPUT(channel)->file_link;
	
	ags_input_open_file(AGS_INPUT(channel),
			    file_link->filename,
			    AGS_AUDIO_FILE_LINK(file_link)->preset,
			    AGS_AUDIO_FILE_LINK(file_link)->instrument,
			    AGS_AUDIO_FILE_LINK(file_link)->sample,
			    AGS_AUDIO_FILE_LINK(file_link)->audio_channel);
	ags_channel_set_format_audio_signal(recycling->audio_signal, format);
      }
    }
  }
}

/**
 * ags_channel_add_remote_channel:
 * @channel: an #AgsChannel
 * @remote_channel: the #AgsRemoteChannel
 *
 * Adds a remote channel.
 *
 * Since: 1.0.0
 */
void
ags_channel_add_remote_channel(AgsChannel *channel, GObject *remote_channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || remote_channel == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* add recall id */    
  pthread_mutex_lock(mutex);

  g_object_ref(G_OBJECT(remote_channel));
  channel->remote_channel = g_list_prepend(channel->remote_channel,
					   remote_channel);
  
  pthread_mutex_unlock(mutex);
}


/**
 * ags_channel_remove_remote_channel:
 * @channel: an #AgsChannel
 * @remote_channel: the #AgsRemoteChannel
 *
 * Removes a remote channel.
 *
 * Since: 1.0.0
 */
void
ags_channel_remove_remote_channel(AgsChannel *channel, GObject *remote_channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || remote_channel == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* remove recall id */    
  pthread_mutex_lock(mutex);

  channel->remote_channel = g_list_remove(channel->remote_channel,
				     remote_channel);
  g_object_unref(G_OBJECT(remote_channel));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_add_recall_id:
 * @channel: an #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 1.0.0
 */
void
ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* add recall id */    
  pthread_mutex_lock(mutex);

  g_object_ref(G_OBJECT(recall_id));
  channel->recall_id = g_list_prepend(channel->recall_id,
				      recall_id);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_remove_recall_id:
 * @channel: an #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 1.0.0
 */
void
ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* remove recall id */    
  pthread_mutex_lock(mutex);

  channel->recall_id = g_list_remove(channel->recall_id,
				     recall_id);
  g_object_unref(G_OBJECT(recall_id));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_add_recall_container:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 1.0.0
 */
void
ags_channel_add_recall_container(AgsChannel *channel, GObject *recall_container)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall_container == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* add recall container */    
  pthread_mutex_lock(mutex);

  g_object_ref(G_OBJECT(recall_container));

  channel->container = g_list_prepend(channel->container, recall_container);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_remove_recall_container:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 1.0.0
 */
void
ags_channel_remove_recall_container(AgsChannel *channel, GObject *recall_container)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall_container == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* remove recall container */    
  pthread_mutex_lock(mutex);

  channel->container = g_list_remove(channel->container, recall_container);
  g_object_unref(G_OBJECT(recall_container));
  
  pthread_mutex_unlock(mutex);
}


/**
 * ags_channel_add_recall:
 * @channel: an #AgsChannel
 * @recall: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Adds a recall.
 *
 * Since: 1.0.0
 */
void
ags_channel_add_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  if(channel == NULL || recall == NULL){
    return;
  }

  /* add recall */
  g_object_ref(recall);

  if(play){
    pthread_mutex_lock(channel->play_mutex);

    channel->play = g_list_prepend(channel->play, recall);

    pthread_mutex_unlock(channel->play_mutex);
  }else{
    pthread_mutex_lock(channel->recall_mutex);

    channel->recall = g_list_prepend(channel->recall, recall);

    pthread_mutex_unlock(channel->recall_mutex);
  }
}

/**
 * ags_channel_remove_recall:
 * @channel: an #AgsChannel
 * @recall: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Removes a recall.
 *
 * Since: 1.0.0
 */
void
ags_channel_remove_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  if(channel == NULL || recall == NULL){
    return;
  }

  /* remove recall */
  if(play){
    pthread_mutex_lock(channel->play_mutex);

    channel->play = g_list_remove(channel->play, recall);

    pthread_mutex_unlock(channel->play_mutex);
  }else{
    pthread_mutex_lock(channel->recall_mutex);

    channel->recall = g_list_remove(channel->recall, recall);

    pthread_mutex_unlock(channel->recall_mutex);
  }

  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_channel_remove_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 1.0.0
 */
void
ags_channel_add_pattern(AgsChannel *channel, GObject *pattern)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || pattern == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /*  pattern */
  pthread_mutex_lock(mutex);

  g_object_ref(pattern);
  channel->pattern = g_list_prepend(channel->pattern, pattern);

  pthread_mutex_unlock(mutex);
}


/**
 * ags_channel_remove_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 1.0.0
 */
void
ags_channel_remove_pattern(AgsChannel *channel, GObject *pattern)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || pattern == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* remove pattern */
  pthread_mutex_lock(mutex);

  channel->pattern = g_list_remove(channel->pattern, pattern);
  g_object_unref(G_OBJECT(pattern));

  pthread_mutex_unlock(mutex);
}

GList*
ags_channel_get_recall_by_effect(AgsChannel *channel, gchar *filename, gchar *effect)
{
  AgsRecall *recall;
  GList *list, *tmp;

  list = ags_recall_get_by_effect(channel->play,
				  filename,
				  effect);
  tmp = ags_recall_get_by_effect(channel->play,
				 filename,
				 effect);

  if(tmp != NULL){
    if(list != NULL){
      list = g_list_concat(list,
			   tmp);
    }else{
      list = tmp;
    }
  }

  return(list);
}

GList*
ags_channel_add_ladspa_effect(AgsChannel *channel,
			      gchar *filename,
			      gchar *effect)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLadspa *recall_ladspa;

  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  
  AgsMutexManager *mutex_manager;

  GList *port;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  unsigned long effect_index;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(channel_mutex);

  soundcard = channel->soundcard;
  audio = (AgsAudio *) channel->audio;
  
  pthread_mutex_unlock(channel_mutex);

  /* load plugin */
  ladspa_manager = ags_ladspa_manager_get_instance();
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
							filename, effect);

  effect_index = AGS_BASE_PLUGIN(ladspa_plugin)->effect_index;

  /* ladspa play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);
  g_object_set(G_OBJECT(recall_ladspa),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       NULL);
  AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 (GObject *) recall_ladspa,
			 TRUE);

  /* load */
  ags_recall_ladspa_load(recall_ladspa);
  port = ags_recall_ladspa_load_ports(recall_ladspa);  
  
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_LADSPA_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));
    
    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->play = g_list_append(channel->play, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  /* ladspa recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);
  g_object_set(G_OBJECT(recall_ladspa),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_ladspa,
			 FALSE);
  
  AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
  ags_recall_ladspa_load(recall_ladspa);

  if(port != NULL){
    port = g_list_concat(port,
			 ags_recall_ladspa_load_ports(recall_ladspa));
  }
  
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_LADSPA_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 FALSE);  
  
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->recall = g_list_append(channel->recall, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  return(port);
}

GList*
ags_channel_add_dssi_effect(AgsChannel *channel,
			    gchar *filename,
			    gchar *effect)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallDssi *recall_dssi;

  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;
  
  AgsMutexManager *mutex_manager;

  GList *port;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  unsigned long effect_index;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(channel_mutex);

  soundcard = channel->soundcard;
  audio = (AgsAudio *) channel->audio;
  
  pthread_mutex_unlock(channel_mutex);

  /* load plugin */
  dssi_manager = ags_dssi_manager_get_instance();
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
						  filename, effect);

  effect_index = AGS_BASE_PLUGIN(dssi_plugin)->effect_index;

  /* dssi play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  g_object_set(G_OBJECT(recall_dssi),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       NULL);
  AGS_RECALL(recall_dssi)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 (GObject *) recall_dssi,
			 TRUE);

  /* load */
  ags_recall_dssi_load(recall_dssi);
  port = ags_recall_dssi_load_ports(recall_dssi);
  
  
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_DSSI_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->play = g_list_append(channel->play, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  /* dssi recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  g_object_set(G_OBJECT(recall_dssi),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_dssi,
			 FALSE);
  
  AGS_RECALL(recall_dssi)->flags |= AGS_RECALL_TEMPLATE;
  ags_recall_dssi_load(recall_dssi);

  if(port != NULL){
    port = g_list_concat(port,
			 ags_recall_dssi_load_ports(recall_dssi));
  }
  
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_DSSI_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 FALSE);  
  
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->recall = g_list_append(channel->recall, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  return(port);
}

GList*
ags_channel_add_lv2_effect(AgsChannel *channel,
			   gchar *filename,
			   gchar *effect)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLv2 *recall_lv2;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  AgsMutexManager *mutex_manager;

  GList *uri_node;
  GList *port;

  gchar *uri;
  gchar *str;
  
  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;
  uint32_t effect_index;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(channel_mutex);

  soundcard = channel->soundcard;
  audio = (AgsAudio *) channel->audio;
  
  pthread_mutex_unlock(channel_mutex);

  /* find plugin */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
					       filename, effect);

  effect_index = AGS_BASE_PLUGIN(lv2_plugin)->effect_index;
  uri = lv2_plugin->uri;
  
  /* lv2 play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);

  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  g_object_set(G_OBJECT(recall_lv2),
	       "soundcard", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container", recall_container,
	       NULL);
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 (GObject *) recall_lv2,
			 TRUE);

  /* load */
  ags_recall_lv2_load(recall_lv2);
  port = ags_recall_lv2_load_ports(recall_lv2);
    
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_LV2_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->play = g_list_append(channel->play, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  /* lv2 recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   (GObject *) recall_container);
  
  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  g_object_set(G_OBJECT(recall_lv2),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       NULL);
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 (GObject *) recall_lv2,
			 FALSE);
  
  /* load */
  ags_recall_lv2_load(recall_lv2);
  port = ags_recall_lv2_load_ports(recall_lv2);

  if(port != NULL){
    port = g_list_concat(port,
			 ags_recall_lv2_load_ports(recall_lv2));
  }
  
  /* dummy */
  recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
							      AGS_TYPE_RECALL_RECYCLING_DUMMY,
							      AGS_TYPE_RECALL_LV2_RUN);
  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
  g_object_set(G_OBJECT(recall_channel_run_dummy),
	       "soundcard", soundcard,
	       "recall-container", recall_container,
	       "recall-channel", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 (GObject *) recall_channel_run_dummy,
			 FALSE);  
  
  /* check if connected or running */
  pthread_mutex_lock(channel_mutex);

  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate((AgsRecall *) recall_channel_run_dummy,
				       (AgsRecallID *) recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	channel->recall = g_list_append(channel->recall, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);
      }
      
      /* iterate */
      recall_id = recall_id->next;
    }
  }

  pthread_mutex_unlock(channel_mutex);

  return(port);
}

GList*
ags_channel_real_add_effect(AgsChannel *channel,
			    gchar *filename,
			    gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;
  AgsDssiPlugin *dssi_plugin;
  AgsLv2Plugin *lv2_plugin;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GList *port;
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_channel_add_ladspa_effect(channel,
					 filename,
					 effect);
  }

  if(ladspa_plugin == NULL){
    dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						    filename, effect);
    
    if(dssi_plugin != NULL){
      port = ags_channel_add_dssi_effect(channel,
					 filename,
					 effect);
    }
  }
  
  if(ladspa_plugin == NULL &&
     dssi_plugin == NULL){
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_channel_add_lv2_effect(channel,
					filename,
					effect);
    }
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
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
	       "AgsChannel::add-effect");

    /* add message */
    message = ags_message_envelope_alloc(channel,
					 NULL,
					 doc);

    /* set parameter */
    message->parameter = g_new0(GParameter,
				2);
    message->n_params = 2;
    
    message->parameter[0].name = "filename";
    g_value_init(&(message->parameter[0].value),
		 G_TYPE_STRING);
    g_value_set_string(&(message->parameter[0].value),
		       filename);

    message->parameter[1].name = "effect";
    g_value_init(&(message->parameter[1].value),
		 G_TYPE_STRING);
    g_value_set_string(&(message->parameter[1].value),
		       effect);

    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
  
  return(port);
}

GList*
ags_channel_add_effect(AgsChannel *channel,
		       gchar *filename,
		       gchar *effect)
{
  GList *port;

  //FIXME:JK: make it thread-safe

  g_return_val_if_fail(AGS_IS_CHANNEL(channel), NULL);

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[ADD_EFFECT], 0,
		filename,
		effect,
		&port);
  g_object_unref((GObject *) channel);

  return(port);
}

void
ags_channel_real_remove_effect(AgsChannel *channel,
			       guint nth)
{
  AgsAudio *audio;
  AgsRecall *recall_channel, *recall_channel_run;
  AgsRecallContainer *recall_container;

  AgsMutexManager *mutex_manager;
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GList *automation, *automation_next;
  GList *port;
  GList *list, *list_next;
  
  GList *play, *recall;
  GList *task;

  gchar *specifier;
  
  guint nth_effect;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(channel_mutex);

  audio = (AgsAudio *) channel->audio;

  pthread_mutex_unlock(channel_mutex);

  /*  */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* play */
  pthread_mutex_lock(channel_mutex);

  play = channel->play;
  nth_effect = 0;

  while((play = ags_recall_template_find_all_type(play,
						  AGS_TYPE_RECALL_LADSPA,
						  AGS_TYPE_RECALL_LV2,
						  G_TYPE_NONE)) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play->data)->flags)) != 0){
      nth_effect++;
    }

    if(nth_effect == nth + 1){
      break;
    }
    
    play = play->next;
  }

  pthread_mutex_unlock(channel_mutex);

  /* recall */
  pthread_mutex_lock(channel_mutex);
  
  recall = channel->recall;
  nth_effect = 0;

  while((recall = ags_recall_template_find_all_type(recall,
						    AGS_TYPE_RECALL_LADSPA,
						    AGS_TYPE_RECALL_LV2,
						    G_TYPE_NONE)) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall->data)->flags)) != 0){
      nth_effect++;
    }

    if(nth_effect == nth + 1){
      break;
    }
    
    recall = recall->next;
  }
    
  pthread_mutex_unlock(channel_mutex);

  /* play context */
  /* automation */
  pthread_mutex_lock(channel_mutex);

  port = AGS_RECALL(play->data)->port;

  pthread_mutex_unlock(channel_mutex);
  
  while(port != NULL){
    pthread_mutex_lock(channel_mutex);

    specifier = AGS_PORT(port->data)->specifier;
    
    pthread_mutex_unlock(channel_mutex);

    /* find specifier and remove */
    pthread_mutex_lock(audio_mutex);

    automation = audio->automation;
    
    while((automation = ags_automation_find_specifier(automation,
						      specifier)) != NULL){
      automation_next = automation->next;
      
      ags_audio_remove_automation(audio,
				  automation->data);
      
      
      automation = automation_next;
    }
    
    pthread_mutex_unlock(audio_mutex);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    port = port->next;
    
    pthread_mutex_unlock(channel_mutex);
  }  

  /* remove recall */
  pthread_mutex_lock(channel_mutex);

  recall_container = (AgsRecallContainer *) AGS_RECALL(play->data)->container;
  recall_channel = (AgsRecall *) play->data;
  recall_channel_run = (AgsRecall *) ags_recall_find_template(recall_container->recall_channel_run)->data;
  
  list = recall_container->recall_channel_run;

  while(list != NULL){
    list_next = list->next;
    
    ags_channel_remove_recall(channel,
			      (GObject *) list->data,
			      TRUE);
	  
    list = list_next;
  }

  ags_channel_remove_recall(channel,
			    (GObject *) recall_channel,
			    TRUE);

  pthread_mutex_unlock(channel_mutex);

  ags_channel_remove_recall_container(channel,
				      (GObject *) recall_container);

  /* recall context */
  /* automation */
  pthread_mutex_lock(channel_mutex);

  port = AGS_RECALL(recall->data)->port;

  pthread_mutex_unlock(channel_mutex);
  
  while(port != NULL){
    pthread_mutex_lock(channel_mutex);

    specifier = AGS_PORT(port->data)->specifier;
    
    pthread_mutex_unlock(channel_mutex);

    /* find specifier and remove */
    pthread_mutex_lock(audio_mutex);

    automation = audio->automation;
    
    while((automation = ags_automation_find_specifier(automation,
						      specifier)) != NULL){
      automation_next = automation->next;
      
      ags_audio_remove_automation(audio,
				  automation->data);
      
      
      automation = automation_next;
    }
    
    pthread_mutex_unlock(audio_mutex);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    port = port->next;
    
    pthread_mutex_unlock(channel_mutex);
  }

  /* remove recall */
  pthread_mutex_lock(channel_mutex);

  recall_container = (AgsRecallContainer *) AGS_RECALL(recall->data)->container;
  recall_channel = (AgsRecall *) recall->data;
  recall_channel_run = (AgsRecall *) ags_recall_find_template(recall_container->recall_channel_run)->data;
  
  list = recall_container->recall_channel_run;

  while(list != NULL){
    list_next = list->next;
    
    ags_channel_remove_recall(channel,
			      (GObject *) list->data,
			      FALSE);
	  
    list = list_next;
  }

  ags_channel_remove_recall(channel,
			    (GObject *) recall_channel,
			    FALSE);

  pthread_mutex_unlock(channel_mutex);
  
  ags_channel_remove_recall_container(channel,
				      (GObject *) recall_container);

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
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
	       "AgsChannel::remove-effect");

    /* add message */
    message = ags_message_envelope_alloc(channel,
					 NULL,
					 doc);

    /* set parameter */
    message->parameter = g_new0(GParameter,
				1);
    message->n_params = 1;
    
    message->parameter[0].name = "nth";
    g_value_init(&(message->parameter[0].value),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->parameter[0].value),
		     nth);

    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
}

void
ags_channel_remove_effect(AgsChannel *channel,
			  guint nth)
{
  //FIXME:JK: make it thread-safe
  
  g_return_if_fail(AGS_IS_CHANNEL(channel));

  g_object_ref((GObject *) channel);
  g_signal_emit(G_OBJECT(channel),
		channel_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) channel);
}

/**
 * ags_channel_safe_resize_audio_signal:
 * @channel: an #AgsChannel
 * @length: new frame count length
 *
 * Resize audio data.
 *
 * Since: 1.0.0
 */
void
ags_channel_safe_resize_audio_signal(AgsChannel *channel,
				     guint length)
{
  AgsRecycling *last_recycling;
  AgsRecycling *recycling, *end_recycling;
  AgsAudioSignal *audio_signal;

  AgsMutexManager *mutex_manager;
  
  GList *list;

  guint new_length;
  gdouble factor;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *recycling_mutex;

  if(channel == NULL ||
     channel->first_recycling == NULL){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* get recycling and last recycling */
  pthread_mutex_lock(mutex);
  
  recycling = channel->first_recycling;
  last_recycling = channel->last_recycling;

  pthread_mutex_unlock(mutex);

  /* lookup mutex */
  pthread_mutex_lock(application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) last_recycling);

  pthread_mutex_unlock(application_mutex);

  /* get end recycling */
  pthread_mutex_lock(recycling_mutex);

  end_recycling = last_recycling->next;

  pthread_mutex_unlock(recycling_mutex);

  /* safe resize */
  while(recycling != end_recycling){
    /* lookup mutex */
    pthread_mutex_lock(application_mutex);
  
    mutex_manager = ags_mutex_manager_get_instance();

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);

    pthread_mutex_unlock(application_mutex);

    /* get audio signal */
    pthread_mutex_lock(recycling_mutex);
    
    list = recycling->audio_signal;

    while(list != NULL){
      audio_signal = AGS_AUDIO_SIGNAL(list->data);

      /* safe shrink or grow */
      if(audio_signal->stream_current != NULL){
	if(length < audio_signal->length){
	  factor = audio_signal->length / length;

	  new_length = length + (factor *
				 g_list_position(audio_signal->stream_beginning,
						 audio_signal->stream_current));
	}else{
	  factor = length / audio_signal->length;

	  new_length = length - (factor *
				 g_list_position(audio_signal->stream_beginning,
						 audio_signal->stream_current));
	}
      }else{
	new_length = length;
      }

      /* do it so */
      ags_audio_signal_stream_safe_resize(audio_signal,
					  new_length);

      /* iterate audio signal */
      list = list->next;
    }

    /* iterate recycling */
    recycling = recycling->next;

    pthread_mutex_unlock(recycling_mutex);
  }
}

/**
 * ags_channel_duplicate_recall:
 * @channel: an #AgsChannel that contains the #AgsRecall templates
 * @recall_id: the #AgsRecallID the newly allocated #AgsRecall objects belongs to
 *
 * Duplicate #AgsRecall templates for use with ags_channel_recursive_play(),
 * but ags_channel_recursive_play_init() may call this function for you.
 *
 * Since: 1.0.0
 */
void
ags_channel_duplicate_recall(AgsChannel *channel,
			     AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecall *recall, *copy;
  GList *list_recall, *list_recall_start;
  gboolean do_playback, do_sequencer, do_notation;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(channel == NULL ||
     recall_id == NULL){
    return;
  }
  
#ifdef AGS_DEBUG
  g_message("duplicate channel %d", channel->line);
#endif

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* playback mode */
  pthread_mutex_lock(mutex);

  audio = AGS_AUDIO(channel->audio);
  
  do_playback = FALSE;
  do_sequencer = FALSE;
  do_notation = FALSE;

  if((AGS_RECALL_ID_PLAYBACK & (recall_id->flags)) != 0){
    do_playback = TRUE;
  }

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    do_sequencer = TRUE;
  }

  if((AGS_RECALL_ID_NOTATION & (recall_id->flags)) != 0){
    do_notation = TRUE;
  }
  
  /* recall or play */
  if(recall_id->recycling_context->parent == NULL){
    list_recall = g_list_copy(channel->play);
    list_recall_start = 
      list_recall = g_list_reverse(list_recall);
  }else{
    list_recall = g_list_copy(channel->recall);
    list_recall_start =
      list_recall = g_list_reverse(list_recall);
  }

  /* duplicate */
  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);

    /* ignore initialized or non-runnable AgsRecalls */
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0 ||
       AGS_IS_RECALL_AUDIO(recall) ||
       AGS_IS_RECALL_CHANNEL(recall) ||
       recall->recall_id != NULL){
      list_recall = list_recall->next;
      continue;
    }

    if((do_playback && (AGS_RECALL_PLAYBACK & (recall->flags)) == 0) ||
       (do_sequencer && (AGS_RECALL_SEQUENCER & (recall->flags)) == 0) ||
       (do_notation && (AGS_RECALL_NOTATION & (recall->flags)) == 0)){
      list_recall = list_recall->next;
      //      g_message("%x - %x", recall->flags, recall_id->flags);
      continue;
    }
 
    /* duplicate the recall */
    copy = ags_recall_duplicate(recall, recall_id);

    if(copy == NULL){
      /* iterate */    
      list_recall = list_recall->next;

      continue;
    }
    
#ifdef AGS_DEBUG
    g_message("recall duplicated: %s", G_OBJECT_TYPE_NAME(copy));
#endif
    
    /* set appropriate flag */
    if(do_playback){
      ags_recall_set_flags(copy, AGS_RECALL_PLAYBACK);
    }else if(do_sequencer){
      ags_recall_set_flags(copy, AGS_RECALL_SEQUENCER);
    }else if(do_notation){
      ags_recall_set_flags(copy, AGS_RECALL_NOTATION);
    }

    /* append to AgsAudio */
    ags_channel_add_recall(channel,
			   (GObject *) copy,
			   ((recall_id->recycling_context->parent == NULL) ? TRUE: FALSE));
    
    /* connect */
    ags_connectable_connect(AGS_CONNECTABLE(copy));

    /* notify run */
    ags_recall_notify_dependency(copy, AGS_RECALL_NOTIFY_RUN, 1);

    /* iterate */    
    list_recall = list_recall->next;
  }

  g_list_free(list_recall_start);

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_resolve_recall:
 * @channel: an #AgsChannel
 * @recall_id: appropriate #AgsRecallID
 * 
 * Resolve step of initialization.
 *
 * Since: 1.0.0
 */
void
ags_channel_resolve_recall(AgsChannel *channel,
			   AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get the appropriate lists */
  pthread_mutex_lock(mutex);
  
#ifdef AGS_DEBUG
  g_message("resolve channel %d", channel->line);
#endif

  if(recall_id->recycling_context->parent == NULL){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }

  /* resolve dependencies */
  while((list_recall = ags_recall_find_recycling_context(list_recall, G_OBJECT(recall_id->recycling_context))) != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    ags_recall_resolve_dependencies(recall);

    list_recall = list_recall->next;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_init_recall:
 * @channel: an #AgsChannel that contains the recalls
 * @stage: the stage to init
 * @recall_id: the #AgsRecallID this recall belongs to
 *
 * Prepare #AgsRecall objects to become runnning, ags_channel_recursive_play_init()
 * may call this function for you.
 *
 * Since: 1.0.0
 */
void
ags_channel_init_recall(AgsChannel *channel, gint stage,
			AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL ||
     recall_id == NULL){
    return;
  }  

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* recall or play */
  pthread_mutex_lock(mutex);
  
#ifdef AGS_DEBUG
  g_message("ags_channel_init_recall@%d - channel: %llu %llu\n",
	    stage,
	    (long long unsigned int) channel->audio_channel,
	    (long long unsigned int) channel->pad);  
#endif
    
  if(recall_id->recycling_context->parent == NULL){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }

  /* init recall */
  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if(recall->recall_id == NULL ||
       recall->recall_id->recycling_context == NULL ||
       AGS_IS_RECALL_CHANNEL(recall)){
      list_recall = list_recall->next;
      continue;
    }

    if(recall->recall_id->recycling_context != recall_id->recycling_context){
      if(AGS_IS_INPUT(channel) &&
	 recall->recall_id->recycling_context->parent == NULL){
	AgsRecyclingContext *parent_container;

	parent_container = (AgsRecyclingContext *) ags_recall_id_find_parent_recycling_context(AGS_AUDIO(channel->audio)->recall_id,
											       recall->recall_id->recycling_context);
	
	if(recall_id->recycling_context->parent != parent_container){
	  list_recall = list_recall->next;
  	  continue;
	}
      }else{
	list_recall = list_recall->next;
  	continue;
      }
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
#ifdef AGS_DEBUG
      g_message("recall run init@%d: %s - %x", stage, G_OBJECT_TYPE_NAME(recall), recall->flags);
#endif

      if(stage == 0){
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));

	recall->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(recall);
	recall->flags &= (~AGS_RECALL_REMOVE);
      }else if(stage == 1){
	ags_recall_run_init_inter(recall);
      }else{
	ags_recall_run_init_post(recall);
      }
    }
    
    list_recall = list_recall->next;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_play:
 * @channel: an #AgsChannel
 * @recall_id: appropriate #AgsRecallID
 * @stage: run_pre, run_inter or run_post
 * 
 * Play one single run of @stage step.
 *
 * Since: 1.0.0
 */
void
ags_channel_play(AgsChannel *channel,
		 AgsRecallID *recall_id,
		 gint stage)
{
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *list_start, *list, *list_next;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL ||
     recall_id == NULL ||
     recall_id->recycling_context == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* play or recall */
  pthread_mutex_lock(mutex);

  if(recall_id->recycling_context->parent != NULL){
    g_object_get(channel,
		 "recall", &list_start,
		 NULL);
  }else{
    g_object_get(channel,
		 "play", &list_start,
		 NULL);
  }

  pthread_mutex_unlock(mutex);

  list = list_start;
  
  /* run */
  while(list != NULL){
    guint recall_flags;
    
    list_next = list->next;

    recall = AGS_RECALL(list->data);
    
    if(recall == NULL ||
       !AGS_IS_RECALL(recall)){
      //      if(recall_id->recycling_context->parent != NULL){
      //	channel->recall = g_list_remove(channel->recall,
      //					recall);
      //      }else{
      //	channel->play = g_list_remove(channel->play,
      //				      recall);
      //      }

      list = list_next;
      g_warning("recall == NULL");
      
      continue;
    }

    if(AGS_IS_RECALL_CHANNEL(recall)){
      /* run automation*/
      if(stage == 0){
	ags_recall_automate(recall);
      }

      list = list_next;

      continue;
    }

    if(recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;

      continue;
    }

    recall_flags = recall->flags;
        
    if((AGS_RECALL_TEMPLATE & (recall_flags)) == 0){
#ifdef AGS_DEBUG
      g_message("%s play channel %x:%d @%x -> %x", G_OBJECT_TYPE_NAME(recall), channel, channel->line, recall, recall->recall_id);
#endif

      if((AGS_RECALL_HIDE & (recall_flags)) == 0){
	g_object_ref(recall);
	
	if(stage == 0){
	  AGS_RECALL_GET_CLASS(recall)->run_pre(recall);
	}else if(stage == 1){
	  AGS_RECALL_GET_CLASS(recall)->run_inter(recall);
	}else{
	  AGS_RECALL_GET_CLASS(recall)->run_post(recall);
	}

	g_object_unref(recall);
      }
    }

    list = list_next;
  }

  g_list_free(list_start);
}

void
ags_channel_real_done(AgsChannel *channel,
		      AgsRecallID *recall_id)
{
  AgsRecall *recall;
  
  AgsMutexManager *mutex_manager;
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GList *list, *list_next;

  gboolean is_toplevel;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get context */
  pthread_mutex_lock(mutex);

  if(recall_id->recycling_context->parent == NULL){
    list = channel->play;

    is_toplevel = TRUE;
  }else{
    list = channel->recall;

    is_toplevel = FALSE;
  }

  /* remove recall, run dispose and unref */
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);
    if(recall->recall_id != NULL &&
       recall->recall_id->recycling_context == recall_id->recycling_context){
      g_object_run_dispose(recall);
      ags_channel_remove_recall(channel,
				recall,
				is_toplevel);
      g_object_unref(recall);
    }

    list = list_next;
  }
  
  pthread_mutex_unlock(mutex);

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
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
	       "AgsChannel::done");

    /* add message */
    message = ags_message_envelope_alloc(channel,
					 NULL,
					 doc);

    /* set parameter */
    message->parameter = g_new0(GParameter,
				1);
    message->n_params = 1;
    
    message->parameter[0].name = "recall-id";
    g_value_init(&(message->parameter[0].value),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->parameter[0].value),
		       recall_id);

    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
}

/**
 * ags_channel_done:
 * @channel: an #AgsChannel
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as playing channel is done.
 *
 * Since: 1.0.0
 */
void
ags_channel_done(AgsChannel *channel,
		 AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL ||
     recall_id == NULL ||
     recall_id->recycling_context == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* verify type */
  pthread_mutex_lock(mutex);

  if(!(AGS_IS_CHANNEL(channel) ||
       AGS_IS_RECALL_ID(recall_id))){
    pthread_mutex_unlock(mutex);

    return;
  }

  /* emit */
  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[DONE], 0,
		recall_id);
  g_object_unref(G_OBJECT(channel));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_cancel:
 * @channel: an #AgsChannel
 * @recall_id: and #AgsRecallID
 *
 * Calls for every matching @recall_id ags_recall_cancel()
 *
 * Since: 1.0.0
 */
void
ags_channel_cancel(AgsChannel *channel,
		   AgsRecallID *recall_id)
{
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *list, *list_next;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(channel == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* recall or play */
  pthread_mutex_lock(mutex);

  g_object_ref(recall_id);

  if(recall_id->recycling_context->parent == NULL){
    list = channel->play;
  }else{
    list = channel->recall;
  }

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;
      continue;
    }

    //    g_object_ref(recall_id);
    //    g_object_ref(recall);
    ags_recall_cancel(recall);
    
    list = list_next;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_remove:
 * @channel: an #AgsChannel
 * @recall_id: and #AgsRecallID
 *
 * Calls for every matching @recall_id ags_recall_remove()
 *
 * Since: 1.0.0
 */
void
ags_channel_remove(AgsChannel *channel,
		   AgsRecallID *recall_id)
{
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *list, *list_next;

  gboolean play;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(channel == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* recall or play */
  pthread_mutex_lock(mutex);

  if(recall_id->recycling_context->parent == NULL){
    list = channel->play;
    play = TRUE;
  }else{
    list = channel->recall;
    play = FALSE;
  }

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;
      continue;
    }

    ags_recall_remove(recall);
    ags_channel_remove_recall(channel,
			      (GObject *) recall,
			      play);
    g_object_unref(recall_id);

    list = list_next;
  }

  channel->recall_id = g_list_remove(channel->recall_id,
				     recall_id);
  g_object_unref(recall_id);

  pthread_mutex_unlock(mutex);
}

void
ags_channel_recall_id_set(AgsChannel *output, AgsRecallID *default_recall_id, gboolean ommit_own_channel,
			  guint mode, ...)
{
  AgsAudio *audio;
  AgsChannel *current;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsMutexManager *mutex_manager;

  char *key;
  guint flags;
  guint stage;
  gboolean async_recall;

  va_list va_list;
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *current_mutex;
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) output);
  
  pthread_mutex_unlock(application_mutex);

  /* read variadict arguments */
  va_start(va_list, mode);

  switch(mode){
  case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
    key = va_arg(va_list, char*);
    stage = va_arg(va_list, guint);
    break;
  }

  va_end(va_list);

  /* get audio */
  pthread_mutex_lock(mutex);

  audio = (AgsAudio *) output->audio;
  
  pthread_mutex_unlock(mutex);

  /* lookup mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get recycling container and flags */
  pthread_mutex_lock(audio_mutex);

  recycling_context = default_recall_id->recycling_context;
  flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  /*  */
  if((AGS_AUDIO_ASYNC & (flags)) != 0){
    async_recall = TRUE;
  }else{
    async_recall = FALSE;
  }
  
  switch(async_recall){
  case TRUE:
    {
      /* iterate next */
      pthread_mutex_lock(mutex);
    
      current = output->next_pad;

      pthread_mutex_unlock(mutex);
    
      while(current != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);
  
	current_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) current);
  
	pthread_mutex_unlock(application_mutex);

	/* set run stage */
	pthread_mutex_lock(current_mutex);
      
	recall_id = ags_recall_id_find_parent_recycling_context(current->recall_id,
								recycling_context->parent);

	pthread_mutex_unlock(current_mutex);
      
	if(recall_id == NULL){
	  return;
	}

	switch(mode){
	case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
	  {
	    pthread_mutex_lock(audio_mutex);
      
	    ags_recall_id_set_run_stage(default_recall_id, stage);

	    pthread_mutex_unlock(audio_mutex);
	  }
	  break;
	case AGS_CHANNEL_RECALL_ID_CANCEL:
	  {
	    pthread_mutex_lock(current_mutex);

	    recall_id->flags |= AGS_RECALL_ID_CANCEL;
	  
	    pthread_mutex_unlock(current_mutex);
	  }
	  break;
	}

	pthread_mutex_lock(current_mutex);

	current = current->next_pad;

	pthread_mutex_unlock(current_mutex);
      }

      /* iterate prev */
      pthread_mutex_lock(mutex);

      current = output->prev_pad;

      pthread_mutex_unlock(mutex);

      while(current != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);
  
	current_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) current);
  
	pthread_mutex_unlock(application_mutex);

	/* set run stage */
	pthread_mutex_lock(current_mutex);
      
	recall_id = ags_recall_id_find_parent_recycling_context(current->recall_id,
								recycling_context->parent);

	pthread_mutex_unlock(current_mutex);

	switch(mode){
	case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
	  {
	    pthread_mutex_lock(audio_mutex);

	    ags_recall_id_set_run_stage(default_recall_id, stage);

	    pthread_mutex_unlock(audio_mutex);
	  }
	  break;
	case AGS_CHANNEL_RECALL_ID_CANCEL:
	  {
	    pthread_mutex_lock(current_mutex);

	    recall_id->flags |= AGS_RECALL_ID_CANCEL;

	    pthread_mutex_unlock(current_mutex);
	  }
	  break;
	}

	pthread_mutex_lock(current_mutex);

	current = current->prev_pad;

	pthread_mutex_unlock(current_mutex);
      }
    }
  case FALSE:
    {
      if(ommit_own_channel){
	break;
      }

      pthread_mutex_lock(mutex);
      
      recall_id = ags_recall_id_find_parent_recycling_context(output->recall_id,
							      recycling_context->parent);

      pthread_mutex_unlock(mutex);
      
      if(recall_id == NULL){
	return;
      }

      switch(mode){
      case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
	{
	  pthread_mutex_lock(audio_mutex);
      
	  ags_recall_id_set_run_stage(default_recall_id, stage);

	  pthread_mutex_unlock(audio_mutex);
	}
	break;
      case AGS_CHANNEL_RECALL_ID_CANCEL:
	{
	  pthread_mutex_lock(mutex);
	  
	  recall_id->flags |= AGS_RECALL_ID_CANCEL;
	  
	  pthread_mutex_unlock(mutex);
	}
	break;
      }
    }
  }
}

/**
 * ags_channel_find_port:
 * @channel: an #AgsChannel
 *
 * Retrieve all ports of #AgsChannel
 *
 * Returns: a #GList containing #AgsPort
 *
 * Since: 1.0.0
 */
GList*
ags_channel_find_port(AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  GList *recall;
  GList *list;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
    return(NULL);
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);
 
  /* collect port of playing recall */
  pthread_mutex_lock(mutex);
  
  recall = channel->play;

  list = NULL;
   
  while(recall != NULL){
    if(AGS_RECALL(recall->data)->port != NULL){
      if(list == NULL){
	list = g_list_copy(AGS_RECALL(recall->data)->port);
      }else{
	if(AGS_RECALL(recall->data)->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(AGS_RECALL(recall->data)->port));
	}
      }
    }
     
    recall = recall->next;
  }
 
  /* the same for true recall */
  recall = channel->recall;
  
  while(recall != NULL){
    if(AGS_RECALL(recall->data)->port != NULL){
      if(list == NULL){
	list = g_list_copy(AGS_RECALL(recall->data)->port);
      }else{
	if(AGS_RECALL(recall->data)->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(AGS_RECALL(recall->data)->port));
	}
      }
    }
     
    recall = recall->next;
  }
  
  /*  */
  pthread_mutex_unlock(mutex);

  list = g_list_reverse(list);
   
  return(list);
}

/**
 * ags_channel_set_link:
 * @channel: an #AgsChannel to link
 * @link: an other #AgsChannel to link with
 * @error: you may retrieve a AGS_CHANNEL_ERROR_LOOP_IN_LINK error
 *
 * Change the linking of #AgsChannel objects. Sets link, calls ags_channel_set_recycling()
 * and ags_channel_recursive_reset_recall_ids(). Further it does loop detection and makes
 * your machine running. Thus it adds #AgsRecallID. Invoke only by a task.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
		     GError **error)
{
  AgsChannel *old_channel_link, *old_link_link;
  AgsRecycling *first_recycling, *last_recycling;

  AgsMutexManager *mutex_manager;

  GError *this_error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *link_mutex;

  if(channel == NULL &&
     link == NULL){
    return;
  }

  if(channel != NULL &&
     !AGS_IS_CHANNEL(channel)){
    g_warning("ags_channel_set_link() - unsupported type");
    
    return;
  }

  if(link != NULL &&
     !AGS_IS_CHANNEL(link)){
    g_warning("ags_channel_set_link() - unsupported type");
    
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel and link mutex */
  pthread_mutex_lock(application_mutex);
  
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  link_mutex = ags_mutex_manager_lookup(mutex_manager,
					(GObject *) link);
  
  pthread_mutex_unlock(application_mutex);
  
  if(channel != NULL){
    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    old_channel_link = channel->link;

    pthread_mutex_unlock(channel_mutex);

    /* check no change */
    if(old_channel_link == link){
      return;
    }
  }else{
    /* get some fields */
    pthread_mutex_lock(link_mutex);
    
    old_link_link = link->link;

    pthread_mutex_unlock(link_mutex);

    if(old_link_link == NULL){
      return;
    }

    old_channel_link = NULL;
  }

  if(link != NULL){
    /* get some fields */
    pthread_mutex_lock(link_mutex);
    
    old_link_link = link->link;

    pthread_mutex_unlock(link_mutex);
  }else{
    old_link_link = NULL;
  }

#ifdef AGS_DEBUG
  g_message("set link %x %x", channel, link);
#endif
  
  if(link != NULL){
    old_link_link = link->link;
  }else{
    old_link_link = NULL;
  }

  /* check for a loop */
  if(channel != NULL &&
     link != NULL){
    AgsAudio *audio, *current_audio;
    AgsChannel *output, *input;
    AgsChannel *current_channel;

    GObject *soundcard;
    
    guint audio_flags;
    guint current_audio_channel, current_line;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *current_audio_mutex;
    pthread_mutex_t *current_channel_mutex;

    if(AGS_IS_OUTPUT(channel)){
      /* get audio */
      pthread_mutex_lock(link_mutex);

      audio = AGS_AUDIO(link->audio);

      current_line = link->line;
      current_audio_channel = link->audio_channel;

      pthread_mutex_unlock(link_mutex);

      /* set current */
      current_channel = link;
    }else{
      /* get audio */
      pthread_mutex_lock(channel_mutex);

      audio = AGS_AUDIO(channel->audio);

      current_line = channel->line;
      current_audio_channel = channel->audio_channel;

      pthread_mutex_unlock(channel_mutex);

      /* set current */
      current_channel = channel;
    }
    
    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
    
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(audio_mutex);

    output = audio->output;
    input = audio->input;

    audio_flags = audio->flags;
    
    pthread_mutex_unlock(audio_mutex);

    /*  */
    if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
      current_channel = ags_channel_nth(output,
					current_audio_channel);
    }else{
      current_channel = ags_channel_nth(output,
					current_line);
    }

    if(current_channel != NULL){
      current_channel = current_channel->link;
    }

    if(current_channel != NULL){
      /* get current mutex */
      pthread_mutex_lock(application_mutex);
  
      current_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) current_channel);

      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(current_channel_mutex);
      
      current_audio = AGS_AUDIO(current_channel->audio);

      pthread_mutex_unlock(current_channel_mutex);

      while(current_channel != NULL){
	/* check audio */
	if(current_audio == audio){
	  if(error != NULL){
	    g_set_error(error,
			AGS_CHANNEL_ERROR,
			AGS_CHANNEL_ERROR_LOOP_IN_LINK,
			"failed to link channel");
	  }

	  return;
	}

	/* get current audio mutex */
	pthread_mutex_lock(application_mutex);
  
	current_audio_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) current_audio);

	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(current_audio_mutex);

	output = current_audio->output;
	
	audio_flags = current_audio->flags;

	pthread_mutex_unlock(current_audio_mutex);

	/* get current channel mutex */
	pthread_mutex_lock(application_mutex);
  
	current_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) current_channel);

	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(current_channel_mutex);

	current_audio_channel = current_channel->audio_channel;
	current_line = current_channel->line;

	pthread_mutex_unlock(current_channel_mutex);

	if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	  current_channel = ags_channel_nth(output,
					    current_audio_channel);
	}else{
	  current_channel = ags_channel_nth(output,
					    current_line);
	}

	if(current_channel != NULL){
	  /* get current channel mutex */
	  pthread_mutex_lock(application_mutex);
  
	  current_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							   (GObject *) current_channel);

	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(current_channel_mutex);
	  
	  current_channel = current_channel->link;

	  pthread_mutex_unlock(current_channel_mutex);
	}	  

	if(current_channel != NULL){
	  /* get current channel mutex */
	  pthread_mutex_lock(application_mutex);
  
	  current_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							   (GObject *) current_channel);

	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(current_channel_mutex);
	  
	  current_audio = AGS_AUDIO(current_channel->audio);

	  pthread_mutex_unlock(current_channel_mutex);
	}
      }
    }
  }

  /* set link */
  if(channel != NULL){
    pthread_mutex_lock(channel_mutex);

    channel->link = link;

    pthread_mutex_unlock(channel_mutex);

    if(link != NULL){
      g_object_ref(channel);
      g_object_ref(link);
    }else{
      g_object_unref(channel);
    }
  }  

  if(link != NULL){
    pthread_mutex_lock(link_mutex);

    link->link = channel;

    pthread_mutex_unlock(link_mutex);

    if(channel != NULL){
      /* notify */
      ags_channel_set_link(link,
			   channel,
			   &this_error);
    }else{
      g_object_unref(link);
    }
  }

  /* set old input/output to NULL */
  if(old_link_link != NULL){
    pthread_mutex_t *old_link_link_mutex;

    /* get old link link mutex */
    pthread_mutex_lock(application_mutex);
  
    old_link_link_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) old_link_link);
    
    pthread_mutex_unlock(application_mutex);
    
    /* unset link */
    pthread_mutex_lock(old_link_link_mutex);

    old_link_link->link = NULL;

    pthread_mutex_unlock(old_link_link_mutex);

    g_object_unref(old_link_link);
    
    this_error = NULL;

    /* notify */
    //NOTE:JK: this doesn't for recursive recycling
    ags_channel_set_link(old_link_link,
			 NULL,
			 &this_error);
  }

  /* set old input/output to NULL */
  if(old_channel_link != NULL){
    pthread_mutex_t *old_channel_link_mutex;

    /* get old channel link mutex */
    pthread_mutex_lock(application_mutex);
  
    old_channel_link_mutex = ags_mutex_manager_lookup(mutex_manager,
						      (GObject *) old_channel_link);
    
    pthread_mutex_unlock(application_mutex);

    /* unset link */
    pthread_mutex_lock(old_channel_link_mutex);
    
    old_channel_link->link = NULL;

    pthread_mutex_unlock(old_channel_link_mutex);

    g_object_unref(old_channel_link);
      
    this_error = NULL;
    
    /* notify */
    //NOTE:JK: this doesn't for recursive recycling
    ags_channel_set_link(old_channel_link,
			 NULL,
			 &this_error);
  }

  /* set recycling */
  if(channel != NULL){    
    if(AGS_IS_OUTPUT(channel)){   
      pthread_mutex_lock(channel_mutex);

      first_recycling = channel->first_recycling;
      last_recycling = channel->last_recycling;

      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	ags_channel_set_recycling(link,
				  first_recycling, last_recycling,
				  TRUE, TRUE);
      }

      if(AGS_IS_INPUT(old_channel_link)){
	AgsAudio *old_audio;

	GObject *soundcard;
	GObject *old_soundcard;
	
	guint old_audio_flags;

	pthread_mutex_t *old_audio_mutex;
	pthread_mutex_t *old_channel_link_mutex;
	
	/* get old channel link mutex */
	pthread_mutex_lock(application_mutex);
  
	old_channel_link_mutex = ags_mutex_manager_lookup(mutex_manager,
							  (GObject *) old_channel_link);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(old_channel_link_mutex);

	old_audio = old_channel_link->audio;

	old_soundcard = old_channel_link->soundcard;
	
	pthread_mutex_unlock(old_channel_link_mutex);

	/* get old audio mutex */
	pthread_mutex_lock(application_mutex);
  
	old_audio_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) old_audio);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(old_audio_mutex);

	old_audio_flags = old_audio->flags;
	
	pthread_mutex_unlock(old_audio_mutex);

	/*  */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (old_audio_flags)) != 0){	  
	  soundcard = old_channel_link->soundcard;
	  
      	  first_recycling = g_object_new(AGS_TYPE_RECYCLING,
					 "channel", old_channel_link,
					 "soundcard", old_soundcard,
					 NULL);
	  g_object_ref(first_recycling);
	  last_recycling = first_recycling;

	  ags_channel_set_recycling(old_channel_link,
				    first_recycling, last_recycling,
				    TRUE, TRUE);
	}else{
	  ags_channel_set_recycling(old_channel_link,
				    NULL, NULL,
				    TRUE, TRUE);
	}
      }
      
      //      ags_channel_set_recycling(old_link_link,
      //			NULL, NULL,
      //			TRUE, TRUE);
    }else{
      if(link != NULL){
	pthread_mutex_lock(link_mutex);

	first_recycling = link->first_recycling;
	last_recycling = link->last_recycling;

	pthread_mutex_unlock(link_mutex);

	ags_channel_set_recycling(channel,
				  first_recycling, last_recycling,
				  TRUE, TRUE);

	//	ags_channel_set_recycling(old_channel_link,
	//			  NULL, NULL,
	//			  TRUE, TRUE);

      	ags_channel_set_recycling(old_link_link,
				  NULL, NULL,
				  TRUE, TRUE);
      }else{
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0){
	  first_recycling = g_object_new(AGS_TYPE_RECYCLING,
					 "channel", channel,
					 "soundcard", channel->soundcard,
					 NULL);
	  g_object_ref(first_recycling);
	  last_recycling = first_recycling;

	  ags_channel_set_recycling(channel,
				    first_recycling, last_recycling,
				    TRUE, TRUE);

	  //	  ags_channel_set_recycling(old_channel_link,
	  //			    NULL, NULL,
	  //			    TRUE, TRUE);
	}else{
	  ags_channel_set_recycling(channel,
				    NULL, NULL,
				    TRUE, TRUE);

	  //	  ags_channel_set_recycling(old_channel_link,
	  //			    NULL, NULL,
	  //			    TRUE, TRUE);
	}
      }
    }
  }else{
    if(AGS_IS_INPUT(link)){
      AgsAudio *audio;

      GObject *soundcard;
	
      guint audio_flags;

      pthread_mutex_t *audio_mutex;
      pthread_mutex_t *link_mutex;
      
      /* get old link link mutex */
      pthread_mutex_lock(application_mutex);
  
      link_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) link);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(link_mutex);

      audio = link->audio;

      soundcard = link->soundcard;
	
      pthread_mutex_unlock(link_mutex);

      /* get old audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
	
      pthread_mutex_unlock(audio_mutex);
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	first_recycling = g_object_new(AGS_TYPE_RECYCLING,
				       "channel", link,
				       "soundcard", soundcard,
				       NULL);
	g_object_ref(first_recycling);
	last_recycling = first_recycling;
      
	ags_channel_set_recycling(link,
				  first_recycling, last_recycling,
				  TRUE, TRUE);

	ags_channel_set_recycling(old_link_link,
				  NULL, NULL,
				  TRUE, TRUE);
      }else{
	ags_channel_set_recycling(link,
				  NULL, NULL,
				  TRUE, TRUE);

	//	ags_channel_set_recycling(old_link_link,
	//			  NULL, NULL,
	//			  TRUE, TRUE);
      }
    }
  }


  /* reset soundcard */
  if(AGS_IS_OUTPUT(channel) &&
     AGS_IS_INPUT(link)){
    GParameter *parameter;

    parameter = g_new0(GParameter,
		       1);
    
    parameter[0].name = "soundcard";

    g_value_init(&(parameter[0].value),
		 G_TYPE_OBJECT);
    g_value_set_object(&(parameter[0].value),
		       link->soundcard);
    
    ags_channel_recursive_set_property(channel,
				       parameter, 1);
  }
  
  /* reset recall id */
  ags_channel_recursive_reset_recall_ids(channel, link,
					 old_channel_link, old_link_link);
}

/**
 * ags_channel_set_recycling:
 * @channel: the channel to reset
 * @first_recycling: the recycling to set for channel->first_recycling
 * @last_recycling: the recycling to set for channel->last_recycling
 * @update: reset allthough the AgsRecyclings are still the same
 * @destroy_old: destroy old AgsRecyclings
 *
 * Called by ags_channel_set_link() to handle outdated #AgsRecycling references.
 * Invoke only by a task.
 *
 * Since: 1.0.0
 */
void
ags_channel_set_recycling(AgsChannel *channel,
			  AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			  gboolean update, gboolean destroy_old)
{
  AgsAudio *audio;
  AgsAudio *found_prev, *found_next;
  AgsChannel *prev_channel, *next_channel, *current;
  AgsRecycling *parent;
  AgsRecycling *old_first_recycling, *old_last_recycling;
  AgsRecycling *replace_with_first_recycling, *replace_with_last_recycling;
  AgsRecycling *changed_old_first_recycling, *changed_old_last_recycling;
  AgsRecycling *nth_recycling, *next_recycling, *stop_recycling;
  AgsRecycling *next_first_recycling, *prev_last_recycling;
  AgsRecyclingContext *recycling_context, *old_recycling_context;
  AgsRecallID *current_recall_id;

  AgsMutexManager *mutex_manager;
  
  GList *recall_id;

  guint audio_flags;
  guint flags;
  guint complete_level_first, complete_level_last;
  gboolean is_output;
  gboolean replace_first, replace_last;
  gboolean find_prev, find_next;
  gboolean change_old_last, change_old_first;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *current_mutex;
  pthread_mutex_t *nth_channel_mutex;
  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *nth_recycling_mutex;
 
  auto void ags_channel_set_recycling_ref_and_unref();
  
  auto gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input);
  auto void ags_channel_set_recycling_recursive_output(AgsChannel *output);
  auto void ags_channel_set_recycling_recursive(AgsChannel *input);

  auto void ags_channel_set_recycling_emit_changed_input(AgsChannel *input);
  auto void ags_channel_set_recycling_emit_changed_output(AgsChannel *output);
  auto void ags_channel_set_recycling_emit_changed(AgsChannel *input);

  void ags_channel_set_recycling_ref_and_unref(){
    //TODO:JK: check remove
#if 0
    /* ref new recycling */
    if(first_recycling != NULL){
      nth_recycling = first_recycling;
      
      while(nth_recycling != next_first_recycling){
	//	g_object_ref(G_OBJECT(nth_recycling));
	
	nth_recycling = nth_recycling->next;
      }
    }
    
    /* unref old recycling */
    if(destroy_old && old_first_recycling != NULL){
      nth_recycling = old_first_recycling;
      
      while(nth_recycling != old_last_recycling->next){
	next_recycling = nth_recycling->next;
	
	//	g_object_unref(G_OBJECT(nth_recycling));
	
	nth_recycling = next_recycling;
      }
    }
#endif
  }
  
  gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *nth_channel_prev, *nth_channel_next;

    guint audio_flags;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *input_mutex;
    pthread_mutex_t *next_channel_mutex;
    pthread_mutex_t *prev_channel_mutex;

    /* get input mutex */
    pthread_mutex_lock(application_mutex);
  
    input_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) input);
  
    pthread_mutex_unlock(application_mutex);

    /* check done */
    pthread_mutex_lock(input_mutex);
    
    if(input == NULL ||
       (input->first_recycling == replace_with_first_recycling &&
	input->last_recycling == replace_with_last_recycling)){
      pthread_mutex_unlock(input_mutex);
      
      return(TRUE);
    }

    /* set recycling */
    ags_channel_set_recycling_ref_and_unref();

    if(replace_first){
      input->first_recycling = replace_with_first_recycling;
    }

    if(replace_last){
      input->last_recycling = replace_with_last_recycling;
    }

    /* search for neighboor recyclings */
    audio = input->audio;

    pthread_mutex_unlock(input_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* audio flags */
    pthread_mutex_lock(audio_mutex);

    audio_flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);
    
    if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
      if(find_prev){
	nth_channel_prev = ags_channel_prev_with_recycling(input);

	if(nth_channel_prev != NULL){
#ifdef AGS_DEBUG
	  g_message("found prev");
#endif	  
	  find_prev = FALSE;
	  replace_first = FALSE;

	  if(complete_level_first == 0){
	    found_prev = audio;
	    prev_channel = nth_channel_prev;

	    complete_level_first = 1;
	  }
	}
      }else{
	if(nth_channel_prev == NULL){
	  nth_channel_prev = ags_channel_prev_with_recycling(input);
	}
      }
      
      if(find_next){
	nth_channel_next = ags_channel_next_with_recycling(input);
	
	if(nth_channel_next != NULL){
#ifdef AGS_DEBUG
	  g_message("found next");
#endif	  

	  find_next = FALSE;
	  replace_last = FALSE;
	  
	  if(complete_level_last == 0){
	    found_next = audio;
	    next_channel = nth_channel_next;
	    
	    complete_level_last = 1;
	  }
	}
      }else{
	if(nth_channel_next == NULL){
	  nth_channel_next = ags_channel_next_with_recycling(input);
	}
      }

      if(prev_channel != NULL){
	if(next_channel == NULL){
	  if(replace_with_last_recycling == NULL){
	    /* get prev channel mutex */
	    pthread_mutex_lock(application_mutex);
  
	    prev_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							  (GObject *) prev_channel);
  
	    pthread_mutex_unlock(application_mutex);

	    /* prev channel */
	    pthread_mutex_lock(prev_channel_mutex);
	    
	    replace_with_last_recycling = prev_channel->last_recycling;

	    pthread_mutex_unlock(prev_channel_mutex);
	    
	    /* continues flag */
	    find_next = FALSE;
	  }
	}else{
	  /* get prev and next channel mutex */
	  pthread_mutex_lock(application_mutex);
  
	  prev_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							(GObject *) prev_channel);
	  next_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							(GObject *) next_channel);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  /* prev channel */
	  pthread_mutex_lock(prev_channel_mutex);
	  
	  replace_with_last_recycling = prev_channel->last_recycling;

	  pthread_mutex_unlock(prev_channel_mutex);

	  /* next channel */
	  pthread_mutex_lock(next_channel_mutex);

	  replace_with_first_recycling = next_channel->first_recycling;

	  pthread_mutex_unlock(next_channel_mutex);
	}
      }else{
	if(next_channel != NULL){
	  if(replace_with_first_recycling == NULL){
	    /* get prev and next channel mutex */
	    pthread_mutex_lock(application_mutex);
  
	    next_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
							  (GObject *) next_channel);
  
	    pthread_mutex_unlock(application_mutex);

	    /* next channel */
	    pthread_mutex_lock(next_channel_mutex);
	    
	    replace_with_first_recycling = next_channel->first_recycling;

	    pthread_mutex_unlock(next_channel_mutex);

	    /* continues flag */
	    find_prev = FALSE;
	  }
	}
      }
    }
    
    if(replace_first || replace_last){
      return(FALSE);
    }else{
      return(TRUE);
    }
  }
  
  void ags_channel_set_recycling_recursive_output(AgsChannel *output){
    AgsAudio *audio;
    AgsChannel *input;
    AgsChannel *link;
    AgsChannel *nth_channel_prev, *nth_channel_next;
    AgsRecycling *first_recycling, *last_recycling;

    guint flags;
    guint audio_channel;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *output_mutex;
    
    if(output == NULL){
      return;
    }

    /* get output mutex */
    pthread_mutex_lock(application_mutex);
  
    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
  
    pthread_mutex_unlock(application_mutex);

    /* get audio and audio channel */
    pthread_mutex_lock(output_mutex);

    audio = AGS_AUDIO(output->audio);

    audio_channel = output->audio_channel;

    pthread_mutex_unlock(output_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get input and flags */    
    pthread_mutex_lock(audio_mutex);

    input = audio->input;
    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);

    /* update input AgsRecallIDs */
    //TODO:JK: check for compliance
    //    ags_recall_id_reset_recycling(output->recall_id,
    //				  output->first_recycling,
    //				  replace_with_first_recycling, replace_with_last_recycling);
    ags_channel_set_recycling_ref_and_unref();

    if(replace_last){      
      /* do it so */
      pthread_mutex_lock(output_mutex);
      
      output->last_recycling = replace_with_last_recycling;

      pthread_mutex_unlock(output_mutex);
    }

    /* last recycling */
    if(replace_first){
      /* do it so */
      pthread_mutex_lock(output_mutex);
      
      output->first_recycling = replace_with_first_recycling;

      pthread_mutex_unlock(output_mutex);
    }

    /* deeper level */
    pthread_mutex_lock(output_mutex);

    link = output->link;

    pthread_mutex_unlock(output_mutex);
    
    if(link != NULL){
      ags_channel_set_recycling_recursive(link);
    }
  }

  void ags_channel_set_recycling_recursive(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    guint flags;
    guint audio_channel, line;
    gboolean completed;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *input_mutex;

    if(input == NULL){
      return;
    }

    /* get input mutex */
    pthread_mutex_lock(application_mutex);
  
    input_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) input);
  
    pthread_mutex_unlock(application_mutex);

    /* get audio and audio channel */    
    pthread_mutex_lock(input_mutex);

    audio = AGS_AUDIO(input->audio);

    audio_channel = input->audio_channel;
    line = input->line;

    pthread_mutex_unlock(input_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get output */
    pthread_mutex_lock(audio_mutex);

    output = audio->output;
    flags = audio->flags;
        
    pthread_mutex_unlock(audio_mutex);

    /* AgsInput */
    completed = ags_channel_set_recycling_recursive_input(input);

    if(completed){
      return;
    }
    
    /* AgsOutput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	output = ags_channel_nth(output, audio_channel);
      }else{
	output = ags_channel_nth(output, line);
      }
      
      ags_channel_set_recycling_recursive_output(output);
    }
  }
  
  void ags_channel_set_recycling_emit_changed_input(AgsChannel *input){
    AgsRecycling *input_first_recycling, *input_last_recycling;
    
    pthread_mutex_t *input_mutex;

    if(input == NULL){
      return;
    }

    /* get input mutex */
    pthread_mutex_lock(application_mutex);
  
    input_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) input);
  
    pthread_mutex_unlock(application_mutex);

    /* get current recycling */
    pthread_mutex_lock(input_mutex);

    input_first_recycling = input->first_recycling;
    input_last_recycling = input->last_recycling;

    pthread_mutex_unlock(input_mutex);
    
    /* emit changed */
    if(input != channel){
      ags_channel_recycling_changed(input,
				    changed_old_first_recycling, changed_old_last_recycling,
				    input_first_recycling, input_last_recycling,
				    old_first_recycling, old_last_recycling,
				    first_recycling, last_recycling);
    }
  }

  void ags_channel_set_recycling_emit_changed_output(AgsChannel *output){
    AgsChannel *link;
    AgsRecycling *output_first_recycling, *output_last_recycling;
    
    pthread_mutex_t *output_mutex;

    if(output == NULL){
      return;
    }

    /* get output mutex */
    pthread_mutex_lock(application_mutex);
  
    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
  
    pthread_mutex_unlock(application_mutex);

    /* get current recycling */
    pthread_mutex_lock(output_mutex);

    link = output->link;

    output_first_recycling = output->first_recycling;
    output_last_recycling = output->last_recycling;

    pthread_mutex_unlock(output_mutex);
      
    /* emit changed */
    if(output != channel){
      ags_channel_recycling_changed(output,
				    changed_old_first_recycling, changed_old_last_recycling,
				    output_first_recycling, output_last_recycling,
				    old_first_recycling, old_last_recycling,
				    first_recycling, last_recycling);
    }
    
    if(link != NULL){
      ags_channel_set_recycling_emit_changed(link);
    }
  }

  void ags_channel_set_recycling_emit_changed(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    guint flags;
    guint audio_channel, line;

    pthread_mutex_t *input_mutex;

    if(input == NULL){
      return;
    }

    /* get input mutex */
    pthread_mutex_lock(application_mutex);
  
    input_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) input);
  
    pthread_mutex_unlock(application_mutex);

    /* get audio and audio channel */
    pthread_mutex_lock(input_mutex);

    audio = AGS_AUDIO(input->audio);

    audio_channel = input->audio_channel;
    line = input->line;

    pthread_mutex_unlock(input_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get output */
    pthread_mutex_lock(audio_mutex);

    output = audio->output;
    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);
    
    /* AgsInput */
    ags_channel_set_recycling_emit_changed_input(input);

    /* higher level */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	output = ags_channel_nth(output, audio_channel);
      }else{
	output = ags_channel_nth(output, line);
      }
      
      ags_channel_set_recycling_emit_changed_output(output);
    }

    return;
  }

  /* entry point */
  if(channel == NULL){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);
  
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(channel_mutex);

  audio = AGS_AUDIO(channel->audio);
  
  pthread_mutex_unlock(channel_mutex);
  
  /* fix first or last recycling if needed */
  if(first_recycling == NULL && last_recycling != NULL){
    first_recycling = last_recycling;
  }

  if(last_recycling == NULL && first_recycling != NULL){
    last_recycling = first_recycling;
  }

  /* set old recycling */
  old_first_recycling = channel->first_recycling;
  old_last_recycling = channel->last_recycling;

  if(!update && old_first_recycling == first_recycling && old_last_recycling == last_recycling){
    return;
  }

  /* initialising */
  found_prev = NULL;
  found_next = NULL;

  parent = NULL;

  prev_channel = NULL;
  next_channel = NULL;

  replace_with_first_recycling = first_recycling;
  replace_with_last_recycling = last_recycling;

  changed_old_first_recycling = NULL;
  changed_old_last_recycling = NULL;

  complete_level_first = 0;
  complete_level_last = 0;
  
  replace_first = TRUE;
  replace_last = TRUE;

  find_next = TRUE;
  find_prev = TRUE;

  change_old_first = TRUE;
  change_old_last = TRUE;

  if((old_first_recycling == first_recycling)){
    if(!update){
      if(old_last_recycling == last_recycling){
	return;
      }

      replace_first = FALSE;
    }
  }

  if((old_last_recycling == last_recycling)){
    if(!update){
      replace_last = FALSE;
    }
  }

  /* set recycling - update AgsChannel */
  if(AGS_IS_INPUT(channel)){
    ags_channel_set_recycling_recursive(channel);    
  }else{
    ags_channel_set_recycling_recursive_output(channel);
  }

  pthread_mutex_lock(channel_mutex);

  channel->first_recycling = first_recycling;
  channel->last_recycling = last_recycling;

  pthread_mutex_unlock(channel_mutex);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* audio flags */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  /* join now the retrieved recyclings */
  next_first_recycling = NULL;
  prev_last_recycling = NULL;
  
  if(!(AGS_IS_INPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0)){
    if(first_recycling != NULL){
      if(prev_channel != NULL){
	/* get prev channel mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) prev_channel);
  
	pthread_mutex_unlock(application_mutex);

	/* get prev channel last recycling */
	pthread_mutex_lock(nth_channel_mutex);

	prev_last_recycling = prev_channel->last_recycling;

	pthread_mutex_unlock(nth_channel_mutex);
	
	if(prev_last_recycling != NULL){
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) prev_last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* prev channel last recycling next */
	  pthread_mutex_lock(nth_recycling_mutex);
	  
	  prev_last_recycling->next = first_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);

	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) first_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* first recycling prev */	  
	  pthread_mutex_lock(nth_recycling_mutex);

	  first_recycling->prev = prev_last_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}else{
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) first_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* first recycling prev */	  
	  pthread_mutex_lock(nth_recycling_mutex);

	  first_recycling->prev = NULL;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}
      }else{
	/* get nth recycling mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) first_recycling);
  
	pthread_mutex_unlock(application_mutex);

	/* first recycling prev */	  
	pthread_mutex_lock(nth_recycling_mutex);

	first_recycling->prev = NULL;

	pthread_mutex_unlock(nth_recycling_mutex);
      }
    
      if(next_channel != NULL){
	/* get next channel mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) next_channel);
  
	pthread_mutex_unlock(application_mutex);
	
	/* get prev channel last recycling */
	pthread_mutex_lock(nth_channel_mutex);

	next_first_recycling = next_channel->first_recycling;

	pthread_mutex_unlock(nth_channel_mutex);

	if(next_first_recycling != NULL){
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) next_first_recycling);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  /* next channel first recycling next */
	  pthread_mutex_lock(nth_recycling_mutex);
	  
	  next_first_recycling->prev = last_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);
	  
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* last recycling next */	  
	  pthread_mutex_lock(nth_recycling_mutex);

	  last_recycling->next = next_first_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}else{
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* last recycling next */	  
	  pthread_mutex_lock(nth_recycling_mutex);

	  last_recycling->next = NULL;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}
      }else{
	/* get nth recycling mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) last_recycling);
  
	pthread_mutex_unlock(application_mutex);

	/* last recycling next */	  
	pthread_mutex_lock(nth_recycling_mutex);

	last_recycling->next = NULL;

	pthread_mutex_unlock(nth_recycling_mutex);
      }
    }else{
      gboolean link_next, link_prev;

      if(prev_channel != NULL){
	/* get prev channel mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) prev_channel);
  
	pthread_mutex_unlock(application_mutex);

	/*  */
	pthread_mutex_lock(nth_channel_mutex);

	prev_last_recycling = prev_channel->last_recycling;
	
	pthread_mutex_unlock(nth_channel_mutex);

	if(prev_last_recycling != NULL){
	  link_next = TRUE;
	}else{
	  link_next = FALSE;
	}
      }else{
	link_next = FALSE;
      }
    
      if(next_channel != NULL){
	/* get prev channel mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) next_channel);
  
	pthread_mutex_unlock(application_mutex);

	/*  */
	pthread_mutex_lock(nth_channel_mutex);

	next_first_recycling = next_channel->first_recycling;
	
	pthread_mutex_unlock(nth_channel_mutex);
	
	if(next_first_recycling != NULL){
	  link_prev = TRUE;
	}else{
	  link_prev = FALSE;
	}
      }else{
	link_prev = FALSE;
      }
    
      if(link_next){
	if(link_prev){
	  AgsRecycling *first_recycling, *last_recycling;

	  /* get first recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) next_channel);
  
	  pthread_mutex_unlock(application_mutex);

	  pthread_mutex_lock(nth_channel_mutex);

	  first_recycling = next_channel->first_recycling;
	  
	  pthread_mutex_unlock(nth_channel_mutex);
	  
	  /* get last recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) prev_channel);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  pthread_mutex_lock(nth_channel_mutex);

	  last_recycling = prev_channel->last_recycling;
	  
	  pthread_mutex_unlock(nth_channel_mutex);

	  /*
	   * 
	   */
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) next_first_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* next channel first recycling next */
	  pthread_mutex_lock(nth_recycling_mutex);

	  next_first_recycling->prev = last_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);

	  /*
	   * 
	   */
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) prev_last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* prev channel last recycling next */
	  pthread_mutex_lock(nth_recycling_mutex);

	  prev_last_recycling->next = first_recycling;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}else{
	  /* get nth recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) prev_last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /* prev channel last recycling next */
	  pthread_mutex_lock(nth_recycling_mutex);

	  prev_last_recycling->next = NULL;

	  pthread_mutex_unlock(nth_recycling_mutex);
	}
      }else if(link_prev){
	/* get nth recycling mutex */
	pthread_mutex_lock(application_mutex);
  
	nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) next_first_recycling);
  
	pthread_mutex_unlock(application_mutex);

	/* next channel first recycling next */
	pthread_mutex_lock(nth_recycling_mutex);

	next_first_recycling->prev = NULL;

	pthread_mutex_unlock(nth_recycling_mutex);
      }
    }
  }

  /* find and set parent */
  if(first_recycling != NULL){
    AgsAudio *audio;
    AgsChannel *output;

    guint audio_channel, line;
    
    pthread_mutex_t *audio_mutex;
    
    /* find parent */
    parent = NULL;
    
    if(AGS_IS_OUTPUT(channel)){
      pthread_mutex_lock(channel_mutex);
      
      current = channel->link;

      pthread_mutex_unlock(channel_mutex);
    }else{
      current = channel;
    }
    
    while(current != NULL &&
	  parent == NULL){
      /* get current mutex */
      pthread_mutex_lock(application_mutex);
  
      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);
      
      /* get audio */
      pthread_mutex_lock(current_mutex);
      
      audio = (AgsAudio *) current->audio;

      audio_channel = current->audio_channel;
      line = current->line;

      pthread_mutex_unlock(current_mutex);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* get nth */
      pthread_mutex_lock(audio_mutex);
      
      output = audio->output;

      audio_flags = audio->flags;
      
      pthread_mutex_unlock(audio_mutex);

      /* get current */
      if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	current = ags_channel_nth(output,
				  audio_channel);
      }else{
	current = ags_channel_nth(output,
				  line);
      }

      /* get current mutex */
      pthread_mutex_lock(application_mutex);
  
      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* check if parent found */
      if(current != NULL){
	pthread_mutex_lock(current_mutex);
	
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0){
	  /* set parent */
	  parent = current->first_recycling;
	}

	current = current->link;

	pthread_mutex_unlock(current_mutex);
      }else{
	break;
      }
    }
    
    /* apply parent */
    if(update){
      nth_recycling = first_recycling;

      /* get recycling mutex */
      pthread_mutex_lock(application_mutex);
  
      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) last_recycling);
  
      pthread_mutex_unlock(application_mutex);

      /*  */
      pthread_mutex_lock(recycling_mutex);
      
      stop_recycling = last_recycling->next;

      pthread_mutex_unlock(recycling_mutex);
    }else{
      if(first_recycling == old_first_recycling){
	nth_recycling = old_last_recycling;

	/* get recycling mutex */
	pthread_mutex_lock(application_mutex);
  
	recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) last_recycling);
  
	pthread_mutex_unlock(application_mutex);

	/*  */
	pthread_mutex_lock(recycling_mutex);
	
	stop_recycling = last_recycling->next;

	pthread_mutex_unlock(recycling_mutex);
      }else{
	nth_recycling = first_recycling;

	if(old_first_recycling != NULL){
	  /* get recycling mutex */
	  pthread_mutex_lock(application_mutex);
  
	  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) last_recycling);
  
	  pthread_mutex_unlock(application_mutex);

	  /*  */
	  pthread_mutex_lock(recycling_mutex);
	  
	  stop_recycling = old_first_recycling->next;

	  pthread_mutex_unlock(recycling_mutex);
	}else{
	  stop_recycling = NULL;
	}
      }
    }

    /* parent - do it so */
    while(nth_recycling != stop_recycling){
      /* get nth recycling mutex */
      pthread_mutex_lock(application_mutex);
  
      nth_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) nth_recycling);
  
      pthread_mutex_unlock(application_mutex);

      /* set parent and iterate */
      pthread_mutex_lock(nth_recycling_mutex);
      
      nth_recycling->parent = parent;

      nth_recycling = nth_recycling->next;

      pthread_mutex_unlock(nth_recycling_mutex);
    }
  }

  /* add recall id */
  pthread_mutex_lock(audio_mutex);

  recall_id = audio->recall_id;

  pthread_mutex_unlock(audio_mutex);

  while(recall_id != NULL){
    AgsRecyclingContext *recycling_context;
    
    pthread_mutex_lock(audio_mutex);

    if(AGS_RECALL_ID(recall_id->data)->recycling != NULL &&
       AGS_IS_OUTPUT(AGS_RECYCLING(AGS_RECALL_ID(recall_id->data)->recycling)->channel)){
      recall_id = recall_id->next;

      pthread_mutex_unlock(audio_mutex);
	    
      continue;
    }

    recycling_context = AGS_RECALL_ID(recall_id->data)->recycling_context;
    
    pthread_mutex_unlock(audio_mutex);
	    
    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling", first_recycling,
				     "recycling-context", recycling_context,
				     NULL);
	  
    ags_channel_add_recall_id(channel,
			      current_recall_id);

    /* iterate */
    pthread_mutex_lock(audio_mutex);

    recall_id = recall_id->next;

    pthread_mutex_unlock(audio_mutex);
  }

  /* update recycling container */
  pthread_mutex_lock(audio_mutex);

  recall_id = audio->recall_id;

  pthread_mutex_unlock(audio_mutex);
    
  while(recall_id != NULL){
    is_output = AGS_IS_OUTPUT(channel);

    /* check recycling context */
    pthread_mutex_lock(audio_mutex);
    
    if(is_output){
      if((AgsRecycling *) AGS_RECALL_ID(recall_id->data)->recycling != old_first_recycling){
	recall_id = recall_id->next;

	pthread_mutex_unlock(audio_mutex);
	
	continue;
      }
    }else{
      if((AgsRecycling *) AGS_RECALL_ID(recall_id->data)->recycling != old_first_recycling){
	recall_id = recall_id->next;

	pthread_mutex_unlock(audio_mutex);
	
	continue;
      }
    }

    old_recycling_context = (AgsRecyclingContext *) AGS_RECALL_ID(recall_id->data)->recycling_context;
    
    pthread_mutex_unlock(audio_mutex);
        
    /* reset recycling context */
    recycling_context = ags_recycling_context_reset_recycling(old_recycling_context,
							      old_first_recycling, old_last_recycling,
							      first_recycling, last_recycling);

    if(recycling_context != NULL){
      pthread_mutex_lock(audio_mutex);
      
      recycling_context->parent = old_recycling_context->parent;

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);
    }
    
    ags_audio_remove_recycling_context(audio,
				       (GObject *) old_recycling_context);

    /* set recycling-context */
    pthread_mutex_lock(audio_mutex);
    
    g_object_set(recall_id->data,
		 "recycling-context", recycling_context,
		 NULL);

    pthread_mutex_unlock(audio_mutex);

    /* reset recycling context */
    ags_channel_recursive_reset_recycling_context(channel,
						  old_recycling_context,
						  recycling_context);

    /* iterate */
    pthread_mutex_lock(audio_mutex);

    recall_id = recall_id->next;

    pthread_mutex_unlock(audio_mutex);
  }
  
  /* emit changed */
  changed_old_first_recycling = NULL;
  changed_old_last_recycling = NULL;
  
  is_output = AGS_IS_OUTPUT(channel);

  if(!is_output){
    AgsChannel *output;

    if(old_first_recycling != NULL){
      changed_old_first_recycling = old_first_recycling;
      changed_old_last_recycling = old_last_recycling;
    }
    
    ags_channel_set_recycling_emit_changed_input(channel);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) == 0){
      output = audio->output;

      /* get matching output */
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	guint audio_channel;
	
	pthread_mutex_lock(channel_mutex);
	
	audio_channel = channel->audio_channel;

	pthread_mutex_unlock(channel_mutex);
	
	output = ags_channel_nth(output, audio_channel);
      }else{
	guint line;

	pthread_mutex_lock(channel_mutex);

	line = channel->line;

	pthread_mutex_unlock(channel_mutex);
	
	output = ags_channel_nth(output, line);
      }

      /* emit */
      ags_channel_set_recycling_emit_changed_output(output);
    }
  }else{
    ags_channel_set_recycling_emit_changed_output(channel);
  }
}

/**
 * ags_channel_recursive_reset_recycling_context:
 * @channel: the channel to reset
 * @old_recycling_context: the old recycling container context
 * @recycling_context: the new recycling container context
 *
 * Resets the recycling container context. Invoke only by a task.
 *
 * Since: 1.0.0
 */
void
ags_channel_recursive_reset_recycling_context(AgsChannel *channel,
					      AgsRecyclingContext *old_recycling_context,
					      AgsRecyclingContext *recycling_context)
{
  AgsAudio *audio;
  AgsChannel *link;

  AgsMutexManager *mutex_manager;
  
  gboolean is_output;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  auto void ags_channel_reset_recycling_context(GList *recall_id);
  auto void ags_channel_recursive_reset_recycling_context_up(AgsChannel *channel);
  auto void ags_channel_recursive_reset_recycling_context_down_input(AgsChannel *channel);
  auto void ags_channel_recursive_reset_recycling_context_down_output(AgsChannel *channel);

  void ags_channel_reset_recycling_context(GList *recall_id){
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context == old_recycling_context){
	g_object_set(recall_id->data,
		     "recycling-context", recycling_context,
		     NULL);
      }
      
      recall_id = recall_id->next;
    }
  }

  void ags_channel_recursive_reset_recycling_context_up(AgsChannel *channel){
    AgsAudio *audio;
    AgsChannel *current, *output;

    GList *recall_id;
    
    guint flags;
    guint audio_channel, line;
    gboolean is_output;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *current_mutex;
    
    if(channel == NULL){
      return;
    }

    current = channel;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    current_mutex = 
      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    audio_channel = channel->audio_channel;
    line = channel->line;

    audio = (AgsAudio *) channel->audio;

    pthread_mutex_unlock(channel_mutex);

    /*  */
    is_output = AGS_IS_OUTPUT(channel);

    if(is_output){
      /* get audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* prepare jump and reset recycling context */
      pthread_mutex_lock(audio_mutex);
        
      flags = audio->flags;
      
      ags_channel_reset_recycling_context(audio->recall_id);

      pthread_mutex_unlock(audio_mutex);
      
      goto ags_channel_recursive_reset_recycling_context_up_OUTPUT;
    }

    /* reset current channel */
    while(current != NULL){
      /* get some fields and reset recycling context */
      pthread_mutex_lock(current_mutex);

      audio = (AgsAudio *) current->audio;

      audio_channel = current->audio_channel;
      line = current->line;
      
      ags_channel_reset_recycling_context(current->recall_id);

      pthread_mutex_unlock(current_mutex);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* reset recycling container */
      pthread_mutex_lock(audio_mutex);

      flags = audio->flags;
      output = audio->output;
      
      ags_channel_reset_recycling_context(audio->recall_id);

      pthread_mutex_unlock(audio_mutex);

      /* check break */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	break;
      }

      /* get current */
      current = ags_channel_nth(output,
				((AGS_AUDIO_ASYNC & (flags)) ? audio_channel: line));
      
    ags_channel_recursive_reset_recycling_context_up_OUTPUT:

      /* get current mutex */
      pthread_mutex_lock(application_mutex);
  
      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* reset recycling container */
      pthread_mutex_lock(current_mutex);

      ags_channel_reset_recycling_context(current->recall_id);

      /* iterate */
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }

    /* update all in recycling context */
    if(AGS_IS_INPUT(current) &&
       (AGS_AUDIO_ASYNC & (flags)) != 0){
      AgsChannel *input, *link;

      pthread_mutex_t *audio_mutex;
      pthread_mutex_t *input_mutex;

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);
      
      input = audio->input;

      pthread_mutex_unlock(audio_mutex);

      /* get input */
      input = ags_channel_nth(input,
			      audio_channel);

      while(input != NULL){
	/* get input mutex */
	pthread_mutex_lock(application_mutex);
  
	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) input);
  
	pthread_mutex_unlock(application_mutex);
	
	if(input != current){
	  pthread_mutex_lock(input_mutex);
	  
	  link = input->link;
	  ags_channel_reset_recycling_context(input->recall_id);

	  pthread_mutex_unlock(input_mutex);

	  /* traverse the tree */
	  ags_channel_recursive_reset_recycling_context_down_output(link);
	}

	/* iterate */
	pthread_mutex_lock(input_mutex);

	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }
    }
  }

  void ags_channel_recursive_reset_recycling_context_down_input(AgsChannel *output){
    AgsAudio *audio;
    AgsChannel *input;
    AgsChannel *link;

    guint flags;
    guint audio_channel, line;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *output_mutex;
    pthread_mutex_t *input_mutex;
    
    if(output == NULL){
      return;
    }

    /* get output mutex */
    pthread_mutex_lock(application_mutex);
  
    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
  
    pthread_mutex_unlock(application_mutex);

    /* get audio and defaults */
    pthread_mutex_lock(output_mutex);
    
    audio = (AgsAudio *) output->audio;

    audio_channel = output->audio_channel;
    line = output->line;

    pthread_mutex_unlock(output_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* reset recycling container */
    pthread_mutex_lock(audio_mutex);

    input = audio->input;

    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);

    if((AGS_AUDIO_ASYNC & (flags)) != 0){
      input = ags_channel_nth(input,
			      audio_channel);

      while(input != NULL){
	/* get input mutex */
	pthread_mutex_lock(application_mutex);
  
	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) input);
  
	pthread_mutex_unlock(application_mutex);
	
	/* reset recycling container */
	pthread_mutex_lock(input_mutex);
	
	link = input->link;
	
	ags_channel_reset_recycling_context(input->recall_id);

	pthread_mutex_unlock(input_mutex);

	ags_channel_recursive_reset_recycling_context_down_output(link);

	/* iterate */
	pthread_mutex_lock(input_mutex);

	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }
    }else{
      input = ags_channel_nth(input,
			      line);

      /* get input mutex */
      pthread_mutex_lock(application_mutex);
  
      input_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) input);
  
      pthread_mutex_unlock(application_mutex);

      /* reset recycling container */
      pthread_mutex_lock(input_mutex);

      link = input->link;
      
      ags_channel_reset_recycling_context(input->recall_id);

      pthread_mutex_unlock(input_mutex);

      ags_channel_recursive_reset_recycling_context_down_output(link);
    }
  }

  void ags_channel_recursive_reset_recycling_context_down_output(AgsChannel *output){
    AgsAudio *audio;

    guint flags;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *output_mutex;

    if(output == NULL){
      return;
    }

    /* get output mutex */
    pthread_mutex_lock(application_mutex);
  
    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
  
    pthread_mutex_unlock(application_mutex);

    /* get audio and defaults */
    pthread_mutex_lock(output_mutex);
    
    audio = (AgsAudio *) output->audio;

    pthread_mutex_unlock(output_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* check if still reset recycling container */
    pthread_mutex_lock(audio_mutex);

    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);

    if(output != channel &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      return;
    }

    /* reset recycling container - output */
    pthread_mutex_lock(output_mutex);

    ags_channel_reset_recycling_context(output->recall_id);

    pthread_mutex_unlock(output_mutex);

    /* reset recycling container - audio */
    pthread_mutex_lock(audio_mutex);

    ags_channel_reset_recycling_context(audio->recall_id);

    pthread_mutex_unlock(audio_mutex);

    /* traverse the tree */
    ags_channel_recursive_reset_recycling_context_down_input(output);
  }

  /* entry point */
  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* traverse the tree */
  is_output = AGS_IS_OUTPUT(channel);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);
  
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  link = channel->link;
  
  pthread_mutex_unlock(channel_mutex);

  /* recursive reset */
  if(!is_output){
    ags_channel_recursive_reset_recycling_context_down_output(link);
    ags_channel_recursive_reset_recycling_context_up(channel);
  }else{
    ags_channel_recursive_reset_recycling_context_down_output(channel);
    ags_channel_recursive_reset_recycling_context_up(link);
  }
}

/**
 * ags_channel_recycling_changed:
 * @channel: the object recycling changed
 * @old_start_region: first recycling
 * @old_end_region: last recycling
 * @new_start_region: new first recycling
 * @new_end_region: new last recycling
 * @old_start_changed_region: modified link recycling start
 * @old_end_changed_region: modified link recyclig end
 * @new_start_changed_region: replacing link recycling start
 * @new_end_changed_region: replacing link recycling end
 *
 * Modify recycling. Asynchronously only.
 *
 * Since: 1.0.0
 */
void
ags_channel_recycling_changed(AgsChannel *channel,
			      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  if(channel == NULL){
    return;
  }

  /* verify type */
  if(!(AGS_IS_CHANNEL(channel))){
    return;
  }

  /* emit */
  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RECYCLING_CHANGED], 0,
		old_start_region, old_end_region,
		new_start_region, new_end_region,
		old_start_changed_region, old_end_changed_region,
		new_start_changed_region, new_end_changed_region);
  g_object_unref(G_OBJECT(channel));
}

/**
 * ags_channel_recursive_set_property:
 * @channel: the #AgsChannel
 * @parameter: the #GParameter-struct array containing properties
 * @n_params: the length of @parameter array
 *
 * Recursive set property for #AgsChannel.
 *
 * Since: 1.0.0
 */
void
ags_channel_recursive_set_property(AgsChannel *channel,
				   GParameter *parameter, gint n_params)
{
  AgsChannel *link;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *link_mutex;
  
  auto void ags_channel_set_property(AgsChannel *channel,
				     GParameter *parameter, gint n_params);
  auto void ags_channel_recursive_set_property_down(AgsChannel *channel,
						    GParameter *parameter, gint n_params);
  auto void ags_channel_recursive_set_property_down_input(AgsChannel *channel,
							  GParameter *parameter, gint n_params);

  void ags_channel_set_property(AgsChannel *channel,
				GParameter *parameter, gint n_params){
    guint i;

    for(i = 0; i < n_params; i++){
      g_object_set_property((GObject *) channel,
			    parameter[i].name, &(parameter[i].value));
    }
  }
  
  void ags_channel_recursive_set_property_down(AgsChannel *channel,
					       GParameter *parameter, gint n_params){
    if(channel == NULL){
      return;
    }

    ags_channel_set_property(channel,
			     parameter, n_params);
    
    ags_channel_recursive_set_property_down_input(channel,
						  parameter, n_params);
  }
    
  void ags_channel_recursive_set_property_down_input(AgsChannel *channel,
						     GParameter *parameter, gint n_params){
    AgsAudio *audio;
    AgsChannel *input;
    AgsChannel *link;
    
    guint audio_channel;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *input_mutex;
    
    if(channel == NULL){
      return;
    }

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);
    
    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = (AgsAudio *) channel->audio;

    audio_channel = channel->audio_channel;
    
    pthread_mutex_unlock(channel_mutex);

    if(audio == NULL){
      return;
    }

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);

    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    input = audio->input;

    pthread_mutex_unlock(channel_mutex);
    
    /*  */
    input = ags_channel_nth(input,
			    audio_channel);

    while(input != NULL){
      /* get input mutex */
      pthread_mutex_lock(application_mutex);

      input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) input);

      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(input_mutex);

      link = input->link;

      pthread_mutex_unlock(input_mutex);
      
      /* set property */
      ags_channel_set_property(input,
			       parameter, n_params);
      
      ags_channel_recursive_set_property_down(link,
					      parameter, n_params);

      /* iterate */
      pthread_mutex_lock(input_mutex);

      input = input->next_pad;

      pthread_mutex_unlock(input_mutex);
    }
  }
  
  if(channel == NULL){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  link = channel->link;
  
  pthread_mutex_unlock(channel_mutex);
    
  if(AGS_IS_INPUT(channel)){
    ags_channel_set_property(channel,
			     parameter, n_params);
    
    ags_channel_recursive_set_property_down(link,
					    parameter, n_params);
  }else{
    ags_channel_recursive_set_property_down(channel,
					    parameter, n_params);
  }
}

/**
 * ags_channel_recursive_play_init:
 * @channel: the #AgsChannel to prepare
 * @stage: valid values for @stage are: -1 for running all three stages, or the stages 0 through 2 to run
 * just the specified stage. With stage is meant the #AgsRecall run_init_pre, #AgsRecall run_init_inter
 * and #AgsRecall run_init_post stages.
 * @arrange_recall_id: %TRUE if new #AgsRecallID objects should be created
 * @duplicate_templates: %TRUE if the #AgsRecall templates should be duplicated
 * @do_playback: %TRUE if the purpose is a simple playback of the tree, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set %AGS_RECALL_PLAYBACK flag
 * @do_sequencer: %TRUE if the purpose is playing the tree for a sequencer, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the %AGS_RECALL_SEQUENCER flag
 * @do_notation: %TRUE if the purpose is playing the tree for a notation, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the %AGS_RECALL_NOTATION flag
 * @resolve_dependencies: %TRUE if the
 * @recall_id: the initial recall id or %NULL
 *
 * Make the tree ready for a new #AgsPlayback. Only for asynchronous use, take a look at #AgsInitChannel or
 * #AgsInitAudio task.
 *
 * Returns: a new #AgsRecallID
 *
 * Since: 1.0.0
 */
AgsRecallID*
ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				gboolean arrange_recall_id, gboolean duplicate_templates,
				gboolean do_playback, gboolean do_sequencer, gboolean do_notation,
				gboolean resolve_dependencies,
				AgsRecallID *recall_id)
{
  AgsRecyclingContext *recycling_context;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  if(recall_id == NULL){
    recall_id = ags_recall_id_new(channel->first_recycling);

    if(do_playback){
      recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
    }else if(do_sequencer){
      recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
    }else if(do_notation){
      recall_id->flags |= AGS_RECALL_ID_NOTATION;
    }
  }

  if(arrange_recall_id){
    ags_channel_recursive_reset_recall_id(channel,
					  recall_id, TRUE, TRUE,
					  NULL, FALSE, FALSE);
  }

  if(duplicate_templates &&
     resolve_dependencies &&
     (stage == -1 || (stage >= 0 && stage < 3))){
    ags_channel_recursive_init(channel,
			       recall_id,
			       -1, stage,
			       TRUE, TRUE);
  }else{
    if(duplicate_templates){
      ags_channel_recursive_init(channel,
				 recall_id,
				 0, 3,
				 TRUE, TRUE);
    }

    if(resolve_dependencies){
      ags_channel_recursive_init(channel,
				 recall_id,
				 1, 3,
				 TRUE, TRUE);
    }

    if(stage == -1 || (stage >= 0 && stage < 3)){
      ags_channel_recursive_init(channel,
				 recall_id,
				 2, stage,
				 TRUE, TRUE);
    }
  }
  
  return(recall_id);
}

/**
 * ags_channel_recursive_play_threaded:
 * @channel: an #AgsChannel
 * @recall_id: appropriate #AgsRecallID
 * @stage: run_pre, run_inter or run_post
 *
 * Super-threaded version of ags_channel_recursive_play() one single run of @stage step. This
 * function expects to be called by #AgsIteratorThread and having a functional #AgsRecyclingThread
 * setup.
 *
 * Since: 1.0.0
 */
void
ags_channel_recursive_play_threaded(AgsChannel *channel,
				    AgsRecallID *recall_id, gint stage)
{
  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  ags_channel_recursive_run(channel,
			    recall_id,
			    stage,
			    TRUE, TRUE,
			    TRUE);
}

/**
 * ags_channel_recursive_play:
 * @channel: an #AgsChannel
 * @recall_id: appropriate #AgsRecallID
 * @stage: run_pre, run_inter or run_post
 *
 * Call recursively ags_channel_play() and ags_audio_play(). This tree iterator
 * function isn't capable of doing parallel computing tree.
 *
 * Since: 1.0.0
 */
void
ags_channel_recursive_play(AgsChannel *channel,
			   AgsRecallID *recall_id, gint stage)
{
  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  ags_channel_recursive_run(channel,
			    recall_id,
			    stage,
			    TRUE, TRUE,
			    FALSE);
}

/**
 * ags_channel_tillrecycling_cancel:
 * @channel: an #AgsChannel
 * @recall_id: an #AgsRecallID
 *
 * Traverses the tree down and up and calls ags_channel_cancel() for matchin
 * @recall_id.
 *
 * Since: 1.0.0
 */
void
ags_channel_tillrecycling_cancel(AgsChannel *channel,
				 AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  ags_channel_recursive_cancel(channel,
			       recall_id,
			       TRUE, TRUE);
}

/**
 * ags_channel_recursive_reset_recall_ids:
 * @channel: an #AgsChannel that was linked with @link
 * @link: an #AgsChannel that was linked with @channel
 * @old_channel_link: the old link of @channel
 * @old_link_link: the old link of @link
 *
 * Called by ags_channel_set_link() to handle running #AgsAudio objects correctly.
 * This function destroys #AgsRecall objects which were uneeded because they became
 * invalid due to unlinking. By the way it destroys the uneeded #AgsRecallID objects, too.
 * Additionally it creates #AgsRecall and #AgsRecallID objects to prepare becoming a
 * running object (#AgsAudio or #AgsChannel).
 * By the clean up the invalid #AgsRecall objects will be removed.
 * Once the clean up has done ags_channel_recursive_play_init() will be called for every
 * playing instance that was found.
 *
 * Since: 1.0.0
 */
void
ags_channel_recursive_reset_recall_ids(AgsChannel *channel, AgsChannel *link,
				       AgsChannel *old_channel_link, AgsChannel *old_link_link)
{
  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;

  auto void ags_channel_recursive_reset_recall_ids_unset(AgsChannel *channel, AgsChannel *old_channel_link);

  void ags_channel_recursive_reset_recall_ids_unset(AgsChannel *channel, AgsChannel *old_channel_link)
  {
    if(AGS_IS_OUTPUT(channel)){
      GList *start_channel_recall_id, *channel_recall_id;

      pthread_mutex_t *channel_mutex;    
    
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* copy recall id */
      pthread_mutex_lock(channel_mutex);
	
      channel_recall_id =
	start_channel_recall_id = g_list_copy(channel->recall_id);

      pthread_mutex_unlock(channel_mutex);

      while(channel_recall_id != NULL){
	ags_channel_recursive_cancel(old_channel_link,
				     channel_recall_id->data,
				     TRUE, FALSE);
	  
	ags_channel_recursive_reset_recall_id(old_channel_link,
					      NULL, FALSE, FALSE,
					      channel_recall_id->data, TRUE, FALSE);

	channel_recall_id = channel_recall_id->next;
      }

      g_list_free(start_channel_recall_id);
    }else{
      GList *start_old_channel_link_recall_id, *old_channel_link_recall_id;
	
      pthread_mutex_t *old_channel_link_mutex;
    
      /* get old channel link mutex */
      pthread_mutex_lock(application_mutex);

      old_channel_link_mutex = ags_mutex_manager_lookup(mutex_manager,
							(GObject *) old_channel_link);
  
      pthread_mutex_unlock(application_mutex);

      /* copy recall id */
      pthread_mutex_lock(old_channel_link_mutex);
	
      old_channel_link_recall_id =
	start_old_channel_link_recall_id = g_list_copy(old_channel_link->recall_id);

      pthread_mutex_unlock(old_channel_link_mutex);

      while(old_channel_link_recall_id != NULL){
	ags_channel_recursive_cancel(channel,
				     old_channel_link_recall_id->data,
				     TRUE, FALSE);
	  
	ags_channel_recursive_reset_recall_id(channel,
					      NULL, FALSE, FALSE,
					      old_channel_link_recall_id->data, TRUE, FALSE);

	old_channel_link_recall_id = old_channel_link_recall_id->next;
      }

      g_list_free(start_old_channel_link_recall_id);
    }
  }
  
  if(!AGS_IS_CHANNEL(channel) &&
     !AGS_IS_CHANNEL(link)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* first - unset */
  if(old_channel_link != NULL){
    if(channel != NULL){
      ags_channel_recursive_reset_recall_ids_unset(channel, old_channel_link);
    }else{
      g_critical("channel == NULL && old_channel_link != NULL");
    }
  }
  
  if(old_link_link != NULL){    
    if(link != NULL){
      ags_channel_recursive_reset_recall_ids_unset(link, old_link_link);
    }else{
      g_critical("link == NULL && old_link_link != NULL");
    }
  }

  /* next - set */
  if(channel != NULL){
    if(link != NULL){
      if(AGS_IS_OUTPUT(channel)){
	GList *start_channel_recall_id, *channel_recall_id;

	pthread_mutex_t *channel_mutex;    
    
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* copy recall id */
	pthread_mutex_lock(channel_mutex);
	
	channel_recall_id =
	  start_channel_recall_id = g_list_copy(channel->recall_id);

	pthread_mutex_unlock(channel_mutex);

	while(channel_recall_id != NULL){	  
	  ags_channel_recursive_reset_recall_id(link,
						channel_recall_id->data, TRUE, FALSE,
						NULL, FALSE, FALSE);

	  ags_channel_recursive_init(link,
				     channel_recall_id->data,
				     -1, -1,
				     TRUE, FALSE);

	  channel_recall_id = channel_recall_id->next;
	}

	g_list_free(start_channel_recall_id);
      }else{
	GList *start_link_recall_id, *link_recall_id;

	pthread_mutex_t *link_mutex;    
    
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);

	/* copy recall id */
	pthread_mutex_lock(link_mutex);
	
	link_recall_id =
	  start_link_recall_id = g_list_copy(link->recall_id);

	pthread_mutex_unlock(link_mutex);

	while(link_recall_id != NULL){	  
	  ags_channel_recursive_reset_recall_id(channel,
						link_recall_id->data, TRUE, FALSE,
						NULL, FALSE, FALSE);

	  ags_channel_recursive_init(channel,
				     link_recall_id->data,
				     -1, -1,
				     TRUE, FALSE);

	  link_recall_id = link_recall_id->next;
	}

	g_list_free(start_link_recall_id);
      }
    }
  }
}

/**
 * ags_channel_get_level:
 * @channel: the #AgsChannel
 * 
 * Get level.
 * 
 * Returns: the level of @channel as #AgsChannel or %NULL if no parent recycling.
 * 
 * Since: 1.2.7
 */
AgsChannel*
ags_channel_get_level(AgsChannel *channel)
{
  AgsAudio *audio;
  AgsChannel *level;
  
  AgsMutexManager *mutex_manager;

  guint audio_flags;
  guint audio_channel;
  guint input_line;
	
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *level_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* check above recycling */
  level = channel;

  if(AGS_IS_OUTPUT(channel)){
    goto ags_channel_get_level_ITERATE:
  }
	
  while(level != NULL){
    /* get level mutex */
    pthread_mutex_lock(application_mutex);

    level_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) level);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(level_mutex);

    audio = level->audio;

    audio_channel = level->audio_channel;

    input_line = level->line;

    pthread_mutex_unlock(level_mutex);

    /* get some fields */
    pthread_mutex_lock(audio_mutex);
	  
    audio_flags = audio->flags;

    output = audio->output;

    pthread_mutex_unlock(audio_mutex);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
      break;
    }
	  
    if((AGS_AUDIO_ASYNC & audio_flags) != 0){
      level = ags_channel_nth(output,
			      audio_channel);
    }else{
      level = ags_channel_nth(output,
			      input_line);
    }

    if(level == NULL){
      break;
    }

  ags_channel_get_level_ITERATE:
    /* get level mutex */
    pthread_mutex_lock(application_mutex);

    level_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) level);
  
    pthread_mutex_unlock(application_mutex);

    /* iterate */
    pthread_mutex_lock(level_mutex);

    level = level->link;

    pthread_mutex_unlock(level_mutex);
  }

  return(level);
}

/**
 * ags_channel_recursive_reset_recall_id:
 * @channel: the #AgsChannel
 * @valid_recall_id: set #AgsRecallID
 * @valid_set_up: if %TRUE valid set going up, else not
 * @valid_set_down: if %TRUE valid set going down, else not
 * @invalid_recall_id: unset #AgsRecallID
 * @invalid_unset_up: if %TRUE invalid unset going up, else not
 * @invalid_unset_down: if %TRUE invalid unset going down, else not
 * 
 * Recursive reset recall id. Either set @valid_recall_id or unset @invalid_recall_id by specified
 * directions up/down.
 * 
 * Since: 1.2.7
 */
void
ags_channel_recursive_reset_recall_id(AgsChannel *channel,
				      AgsRecallID *valid_recall_id, gboolean valid_set_up, gboolean valid_set_down,
				      AgsRecallID *invalid_recall_id, gboolean invalid_unset_up, gboolean invalid_unset_down)
{  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  auto void ags_channel_recursive_reset_recall_id_set_down(AgsChannel *channel, AgsRecallID *recall_id);
  auto void ags_channel_recursive_reset_recall_id_unset_down(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_reset_recall_id_set_down(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id;
    AgsRecyclingContext *recycling_context;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    if(channel == NULL ||
       recall_id == NULL){
      return;
    }

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* special case */
    if(AGS_IS_INPUT(channel)){
      AgsChannel *link;
      AgsRecycling *first_recycling;
      AgsRecallID *next_recall_id;
      
      pthread_mutex_t *link_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      link = channel->link;

      first_recycling = channel->first_recycling;
      
      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	AgsRecycling *link_first_recycling;
	
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(link_mutex);

	link_first_recycling = link->first_recycling;
	
	pthread_mutex_unlock(link_mutex);

	/* instantiate link recall id */	
	next_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				      "recycling", link_first_recycling,
				      "recycling-context", recycling_context,
				      NULL);

	/* traverse */
	ags_channel_recursive_reset_recall_id_set_down(link,
						       next_recall_id);
      }
      
      /* add recall id - input */
      ags_channel_add_recall_id(channel,
				recall_id);

      /* instantiate audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling", first_recycling,
				     "recycling-context", recycling_context,
				     NULL);
      
      /* add recall id - audio */
      ags_audio_add_recall_id(audio,
			      audio_recall_id);
    }else{
      AgsChannel *input;      
      AgsChannel *link;
      AgsRecycling *first_recycling;
      AgsRecallID *input_recall_id;
      AgsRecallID *next_recall_id;
      AgsRecyclingContext *next_recycling_context;
      
      guint audio_flags;
      guint audio_channel;
      guint output_line;
      
      pthread_mutex_t *input_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      audio_channel = channel->audio_channel;
      output_line = channel->line;

      first_recycling = channel->first_recycling;
      
      pthread_mutex_unlock(channel_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      input = audio->input;
      
      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	input = ags_channel_nth(input,
				audio_channel);

	while(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    AgsRecycling *first_recycling, *last_recycling;
	    AgsRecycling *recycling, *end_recycling;

	    guint64 length;
	    guint64 i;
	    
	    pthread_mutex_t *recycling_mutex;

	    /* get first recycling */
	    pthread_mutex_lock(input_mutex);
	    
	    first_recycling = input->first_recycling;
	    last_recycling = input->last_recycling;

	    pthread_mutex_unlock(input_mutex);

	    /* get recycling mutex */
	    if(last_recycling != NULL){
	      pthread_mutex_lock(application_mutex);
	    
	      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) last_recycling);
	    
	      pthread_mutex_unlock(application_mutex);

	      /* get end recycling */
	      pthread_mutex_lock(recycling_mutex);
      
	      end_recycling = last_recycling->next;

	      pthread_mutex_unlock(recycling_mutex);

	      /* recall - recycling context */
	      length = ags_recycling_position(first_recycling, end_recycling,
					      last_recycling);
	    
	      next_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
						    "length", length,
						    "parent", recycling_context,
						    NULL);

	      /* insert recycling */
	      recycling = first_recycling;
	  
	      for(i = 0; i < length; i++){
		/* get recycling mutex */
		pthread_mutex_lock(application_mutex);

		recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							   (GObject *) recycling);
  
		pthread_mutex_unlock(application_mutex);

		/* insert */
		ags_recycling_context_replace(next_recycling_context,
					      recycling,
					      i);

		/* iterate */
		pthread_mutex_lock(recycling_mutex);

		recycling = recycling->next;

		pthread_mutex_unlock(recycling_mutex);
	      }
	    }else{
	      /* recall - empty recycling context */
	      next_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
						    "length", 0,
						    "parent", recycling_context,
						    NULL);
	    }
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* instantiate input recall id */
	  input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling", first_recycling,
					 "recycling-context", next_recycling_context,
					 NULL);
	  
	  /* get link */
	  pthread_mutex_lock(input_mutex);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* instantiate link recall id */
	    next_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					  "recycling", first_recycling,
					  "recycling-context", next_recycling_context,
					  NULL);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_set_down(link,
							   next_recall_id);
	  }

	  /* add recall id - input */
	  ags_channel_add_recall_id(input,
				    input_recall_id);

	  /* instantiate audio recall id */
	  audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling", first_recycling,
					 "recycling-context", next_recycling_context,
					 NULL);
      
	  /* add recall id - audio */
	  ags_audio_add_recall_id(audio,
				  audio_recall_id);
	  
	  /* iterate */
	  pthread_mutex_lock(input_mutex);
	  
	  input = input->next;

	  pthread_mutex_unlock(input_mutex);
	}
      }else{
	input = ags_channel_nth(input,
				output_line);

	if(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    AgsRecycling *first_recycling, *last_recycling;
	    AgsRecycling *recycling, *end_recycling;

	    guint64 length;
	    guint64 i;
	    
	    pthread_mutex_t *recycling_mutex;

	    /* get first recycling */
	    pthread_mutex_lock(input_mutex);
	    
	    first_recycling = input->first_recycling;
	    last_recycling = input->last_recycling;

	    pthread_mutex_unlock(input_mutex);

	    /* get recycling mutex */
	    if(last_recycling != NULL){
	      pthread_mutex_lock(application_mutex);
	    
	      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							 (GObject *) last_recycling);
	    
	      pthread_mutex_unlock(application_mutex);

	      /* get end recycling */
	      pthread_mutex_lock(recycling_mutex);
      
	      end_recycling = last_recycling->next;

	      pthread_mutex_unlock(recycling_mutex);

	      /* recall - recycling context */
	      length = ags_recycling_position(first_recycling, end_recycling,
					      last_recycling);
	    
	      next_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
						    "length", length,
						    "parent", recycling_context,
						    NULL);

	      /* insert recycling */
	      recycling = first_recycling;
	  
	      for(i = 0; i < length; i++){
		/* get recycling mutex */
		pthread_mutex_lock(application_mutex);

		recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
							   (GObject *) recycling);
  
		pthread_mutex_unlock(application_mutex);

		/* insert */
		ags_recycling_context_replace(next_recycling_context,
					      recycling,
					      i);

		/* iterate */
		pthread_mutex_lock(recycling_mutex);

		recycling = recycling->next;

		pthread_mutex_unlock(recycling_mutex);
	      }
	    }else{
	      /* recall - empty recycling context */
	      next_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
						    "length", 0,
						    "parent", recycling_context,
						    NULL);
	    }
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* instantiate input recall id */
	  input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling", first_recycling,
					 "recycling-context", next_recycling_context,
					 NULL);

	  /* get link */
	  pthread_mutex_lock(input_mutex);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* instantiate link recall id */
	    next_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					  "recycling", first_recycling,
					  "recycling-context", next_recycling_context,
					  NULL);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* add recall id - input */
	  ags_channel_add_recall_id(input,
				    input_recall_id);

	  /* instantiate audio recall id */
	  audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling", first_recycling,
					 "recycling-context", next_recycling_context,
					 NULL);
      
	  /* add recall id - audio */
	  ags_audio_add_recall_id(audio,
				  audio_recall_id);
	}
      }

      /* instantiate audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling", first_recycling,
				     "recycling-context", recycling_context,
				     NULL);
      
      /* add recall id - audio */
      ags_audio_add_recall_id(audio,
			      audio_recall_id);
      
      /* add recall id - output */
      ags_channel_add_recall_id(channel,
				recall_id);
    }
  }

  void ags_channel_recursive_reset_recall_id_unset_down(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id;
    AgsRecyclingContext *recycling_context;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    if(channel == NULL ||
       recall_id == NULL){
      return;
    }
    
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* special case */
    if(AGS_IS_INPUT(channel)){
      AgsChannel *link;
      AgsRecallID *next_recall_id;
      
      pthread_mutex_t *link_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      link = channel->link;

      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);
	
	/* get link recall id */
	pthread_mutex_lock(link_mutex);
	
	next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
							      recycling_context);
	
	pthread_mutex_unlock(link_mutex);

	/* traverse */
	ags_channel_recursive_reset_recall_id_unset_down(link,
							 next_recall_id);
      }
      
      /* remove recall id - input */
      ags_channel_remove_recall_id(channel,
				   recall_id);

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* remove recall id - audio */
      ags_audio_remove_recall_id(audio,
				 audio_recall_id);
    }else{
      AgsChannel *input;      
      AgsChannel *link;
      AgsRecallID *input_recall_id;
      AgsRecallID *next_recall_id;
      AgsRecyclingContext *next_recycling_context;
      
      guint audio_flags;
      guint audio_channel;
      guint output_line;
      
      pthread_mutex_t *input_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      audio_channel = channel->audio_channel;
      output_line = channel->line;

      pthread_mutex_unlock(channel_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      input = audio->input;
      
      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	input = ags_channel_nth(input,
				audio_channel);

	while(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* remove recall id - input */
	  ags_channel_remove_recall_id(input,
				       input_recall_id);

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* remove recall id - audio */
	  ags_audio_remove_recall_id(audio,
				     audio_recall_id);
	  
	  /* iterate */
	  pthread_mutex_lock(input_mutex);
	  
	  input = input->next;

	  pthread_mutex_unlock(input_mutex);
	}
      }else{
	input = ags_channel_nth(input,
				output_line);

	if(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* remove recall id - input */
	  ags_channel_remove_recall_id(input,
				       input_recall_id);

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* remove recall id - audio */
	  ags_audio_remove_recall_id(audio,
				     audio_recall_id);
	}
      }

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* remove recall id - audio */
      ags_audio_remove_recall_id(audio,
				 audio_recall_id);
      
      /* remove recall id - output */
      ags_channel_remove_recall_id(channel,
				   recall_id);
    }
  }

  if(!AGS_IS_CHANNEL(channel)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recall id */
  if(invalid_recall_id != NULL){    
    if(invalid_recall_id->recycling_context == NULL){
      g_critical("unable to handle invalid recall id without recycling context");
    }else{
      g_object_ref(invalid_recall_id);
      g_object_ref(invalid_recall_id->recycling_context);

      if(invalid_unset_down){
	ags_channel_recursive_reset_recall_id_unset_down(channel,
							 invalid_recall_id);
      }

      if(invalid_unset_up){
	AgsAudio *audio;
	AgsChannel *output;
	AgsChannel *current;
	AgsRecallID *recall_id;
	AgsRecyclingContext *recycling_context;

	guint audio_flags;
	guint audio_channel;
	guint input_line;
	
	pthread_mutex_t *audio_mutex;
	pthread_mutex_t *current_mutex;

	current = channel;
	
	/* get recycling context */
	pthread_mutex_lock(channel_mutex);
	
	recycling_context = invalid_recall_id->recycling_context;

	pthread_mutex_unlock(channel_mutex);

	if(AGS_IS_OUTPUT(channel)){
	  /* get some fields */
	  pthread_mutex_lock(channel_mutex);

	  audio = channel->audio;

	  pthread_mutex_unlock(channel_mutex);

	  /* get audio mutex */
	  pthread_mutex_lock(application_mutex);

	  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) audio);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  /* remove of audio */
	  pthread_mutex_lock(audio_mutex);

	  recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(audio_mutex);

	  ags_audio_remove_recall_id(audio,
				     recall_id);

	  /* jump into loop */
	  goto ags_channel_recursive_reset_recall_id_INVALID_GO_UP;
	}

	/* go up */
	while(current != NULL){
	  /* get current mutex */
	  pthread_mutex_lock(application_mutex);

	  current_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) current);
  
	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(current_mutex);

	  audio = current->audio;

	  audio_channel = current->audio_channel;

	  input_line = current->line;

	  pthread_mutex_unlock(current_mutex);

	  /* remove of input */
	  pthread_mutex_lock(current_mutex);

	  recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(current_mutex);

	  ags_channel_remove_recall_id(current,
				       recall_id);	  
	  
	  /* get audio mutex */
	  pthread_mutex_lock(application_mutex);

	  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) audio);
	  
	  pthread_mutex_unlock(application_mutex);
	  
	  /* remove of audio */
	  pthread_mutex_lock(audio_mutex);

	  recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(audio_mutex);

	  ags_audio_remove_recall_id(audio,
				     recall_id);
	  
	  /* get some fields */
	  pthread_mutex_lock(audio_mutex);
	  
	  audio_flags = audio->flags;

	  output = audio->output;

	  pthread_mutex_unlock(audio_mutex);

	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    break;
	  }
	  
	  if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	    current = ags_channel_nth(output,
				      audio_channel);
	  }else{
	    current = ags_channel_nth(output,
				      input_line);
	  }

	  if(current == NULL){
	    break;
	  }
	  
	ags_channel_recursive_reset_recall_id_INVALID_GO_UP:
	  /* get current mutex */
	  pthread_mutex_lock(application_mutex);

	  current_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) current);
  
	  pthread_mutex_unlock(application_mutex);

	  /* remove of output */
	  pthread_mutex_lock(current_mutex);

	  recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(current_mutex);

	  ags_channel_remove_recall_id(current,
				       recall_id);	  

	  /* iterate */
	  pthread_mutex_lock(current_mutex);
	  
	  current = current->link;

	  pthread_mutex_unlock(current_mutex);
	}
      }

      g_object_unref(invalid_recall_id->recycling_context);
      g_object_unref(invalid_recall_id);
    }
  }

  /* add recall id */
  if(valid_recall_id != NULL){
    AgsRecyclingContext *recycling_context;

    recycling_context = valid_recall_id->recycling_context;

    if(recycling_context == NULL){
      AgsChannel *level;

      level = ags_channel_get_level(channel);

      if(level == NULL){
	/* play - recycling context */
	recycling_context = ags_recycling_context_new(1);

	recycling_context->recycling[0] = first_recycling; 
      }else{
	AgsRecycling *first_recycling, *last_recycling;
	AgsRecycling *recycling, *end_recycling;

	guint64 length;
	guint64 i;
	  
	pthread_mutex_t *level_mutex;
	pthread_mutex_t *recycling_mutex;
	  
	/* get current mutex */
	pthread_mutex_lock(application_mutex);

	level_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) level);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(level_mutex);

	first_recycling = level->first_recycling;
	last_recycling = level->last_recycling;
	  
	pthread_mutex_unlock(level_mutex);

	/* get recycling mutex */
	if(last_recycling != NULL){
	  pthread_mutex_lock(application_mutex);
	    
	  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) last_recycling);
	    
	  pthread_mutex_unlock(application_mutex);

	  /* get end recycling */
	  pthread_mutex_lock(recycling_mutex);
      
	  end_recycling = last_recycling->next;

	  pthread_mutex_unlock(recycling_mutex);

	  /* recall - recycling context */
	  length = ags_recycling_position(first_recycling, end_recycling,
					  last_recycling);
	    
	  recycling_context = ags_recycling_context_new(length);

	  /* insert recycling */
	  recycling = first_recycling;
	  
	  for(i = 0; i < length; i++){
	    /* get recycling mutex */
	    pthread_mutex_lock(application_mutex);

	    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						       (GObject *) recycling);
  
	    pthread_mutex_unlock(application_mutex);

	    /* insert */
	    ags_recycling_context_replace(recycling_context,
					  recycling,
					  i);

	    /* iterate */
	    pthread_mutex_lock(recycling_mutex);

	    recycling = recycling->next;

	    pthread_mutex_unlock(recycling_mutex);
	  }
	}else{
	  /* recall - empty recycling context */
	  recycling_context = ags_recycling_context_new(0);
	}	  
      }
    }
    
    if(valid_set_down){
      ags_channel_recursive_reset_recall_id_set_down(channel,
						     valid_recall_id);
    }

    if(valid_set_up){
      AgsAudio *audio;
      AgsChannel *output;
      AgsChannel *current;
      AgsRecycling *first_recycling;
      AgsRecallID *recall_id;

      guint audio_flags;
      guint audio_channel;
      guint input_line;
	
      pthread_mutex_t *audio_mutex;
      pthread_mutex_t *current_mutex;

      current = channel;

      /* get first recycling */
      pthread_mutex_lock(channel_mutex);
      
      first_recycling = channel->first_recycling;

      pthread_mutex_unlock(channel_mutex);

      if(AGS_IS_OUTPUT(channel)){
	if(!valid_set_down){	    
	  /* create audio recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling", first_recycling,
				   "recycling-context", recycling_context,
				   NULL);
	
	  ags_audio_add_recall_id(audio,
				  recall_id);
	}
	
	/* jump into loop */
	goto ags_channel_recursive_reset_recall_id_VALID_GO_UP;
      }
      
      /* go up */
      while(current != NULL){
	/* get current mutex */
	pthread_mutex_lock(application_mutex);

	current_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) current);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(current_mutex);

	audio = current->audio;

	audio_channel = current->audio_channel;

	input_line = current->line;

	pthread_mutex_unlock(current_mutex);

	/* create input recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling", first_recycling,
				 "recycling-context", recycling_context,
				 NULL);

	/* add to input */
	ags_channel_add_recall_id(current,
				  recall_id);	  

	/* get audio mutex */
	pthread_mutex_lock(application_mutex);

	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
	  
	pthread_mutex_unlock(application_mutex);

	/* create audio recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling", first_recycling,
				 "recycling-context", recycling_context,
				 NULL);
	  
	/* add to audio */
	ags_audio_add_recall_id(audio,
				recall_id);
	  
	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	  
	audio_flags = audio->flags;

	output = audio->output;

	pthread_mutex_unlock(audio_mutex);

	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	  break;
	}
	  
	if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	  current = ags_channel_nth(output,
				    audio_channel);
	}else{
	  current = ags_channel_nth(output,
				    input_line);
	}

	if(current == NULL){
	  break;
	}

      ags_channel_recursive_reset_recall_id_VALID_GO_UP:
	/* get current mutex */
	pthread_mutex_lock(application_mutex);

	current_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) current);
  
	pthread_mutex_unlock(application_mutex);

	if(valid_recall_id == recall_id){
	  recall_id = valid_recall_id;

	  if(!valid_set_down){	    
	    /* add provided recall id */
	    ags_channel_add_recall_id(current,
				      recall_id);
	  }
	}else{
	  /* create audio recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling", first_recycling,
				   "recycling-context", recycling_context,
				   NULL);
	  
	  ags_audio_add_recall_id(audio,
				  recall_id);

	  /* create output recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling", first_recycling,
				   "recycling-context", recycling_context,
				   NULL);
	  
	  ags_channel_add_recall_id(current,
				    recall_id);
	}

	
	/* iterate */
	pthread_mutex_lock(current_mutex);
	  
	current = current->link;

	pthread_mutex_unlock(current_mutex);
      }
    }
  }
}

/**
 * ags_channel_recursive_init:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 * @init_up: if %TRUE init going up, else not
 * @init_down: if %TRUE init going down, else not
 * 
 * Recursive initialize the recall id. This means any recall is going to be
 * duplicated, resolved and initialized for @recall_id.
 * 
 * Since: 1.2.7
 */
void
ags_channel_recursive_init(AgsChannel *channel,
			   AgsRecallID *recall_id,
			   gint stage, gint init_stage,
			   gboolean init_up, gboolean init_down)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  auto void ags_channel_recursive_init_down(AgsChannel *channel, AgsRecallID *recall_id);
  auto void ags_channel_recursive_init_up(AgsChannel *channel, AgsRecallID *recall_id);
  
  void ags_channel_recursive_init_down(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id;
    AgsRecyclingContext *recycling_context;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);
    
    /* special case */
    if(AGS_IS_INPUT(channel)){
      AgsChannel *link;
      AgsRecallID *next_recall_id;
      
      pthread_mutex_t *link_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      link = channel->link;
      
      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	AgsRecycling *link_first_recycling;
	
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);

	/* get link recall id */
	pthread_mutex_lock(link_mutex);
	
	next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
							      recycling_context);
	
	pthread_mutex_unlock(link_mutex);

	/* traverse */
	ags_channel_recursive_reset_recall_id_unset_down(link,
							 next_recall_id);
      }
      
      /* init recall - input */
      switch(stage){
      case 0:
	{
	  ags_channel_duplicate_recall(channel,
				       recall_id);
	}
	break;
      case 1:
	{
	  ags_channel_resolve_recall(channel,
				     recall_id);
	}
	break;
      case 2:
	{
	  ags_channel_init_recall(channel, init_stage,
				  recall_id);
	}
	break;
      }

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* init recall - audio */
      switch(stage){
      case 0:
	{
	  ags_audio_duplicate_recall(audio,
				     audio_recall_id);
	}
	break;
      case 1:
	{
	  ags_audio_resolve_recall(audio,
				   audio_recall_id);
	}
	break;
      case 2:
	{
	  ags_audio_init_recall(audio, init_stage,
				audio_recall_id);
	}
	break;
      }
    }else{
      AgsChannel *input;      
      AgsChannel *link;
      AgsRecallID *input_recall_id;
      AgsRecallID *next_recall_id;
      AgsRecyclingContext *next_recycling_context;
      
      guint audio_flags;
      guint audio_channel;
      guint output_line;
      
      pthread_mutex_t *input_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      audio_channel = channel->audio_channel;
      output_line = channel->line;

      pthread_mutex_unlock(channel_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      input = audio->input;
      
      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	input = ags_channel_nth(input,
				audio_channel);

	while(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* init recall - input */
	  switch(stage){
	  case 0:
	    {
	      ags_channel_duplicate_recall(input,
					   input_recall_id);
	    }
	    break;
	  case 1:
	    {
	      ags_channel_resolve_recall(input,
					 input_recall_id);
	    }
	    break;
	  case 2:
	    {
	      ags_channel_init_recall(input, init_stage,
				      input_recall_id);
	    }
	    break;
	  }

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* init recall - audio */
	  switch(stage){
	  case 0:
	    {
	      ags_audio_duplicate_recall(audio,
					 audio_recall_id);
	    }
	    break;
	  case 1:
	    {
	      ags_audio_resolve_recall(audio,
				       audio_recall_id);
	    }
	    break;
	  case 2:
	    {
	      ags_audio_init_recall(audio, init_stage,
				    audio_recall_id);
	    }
	    break;
	  }
	  
	  /* iterate */
	  pthread_mutex_lock(input_mutex);
	  
	  input = input->next;

	  pthread_mutex_unlock(input_mutex);
	}
      }else{
	input = ags_channel_nth(input,
				output_line);

	if(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* init recall - input */
	  switch(stage){
	  case 0:
	    {
	      ags_channel_duplicate_recall(input,
					   input_recall_id);
	    }
	    break;
	  case 1:
	    {
	      ags_channel_resolve_recall(input,
					 input_recall_id);
	    }
	    break;
	  case 2:
	    {
	      ags_channel_init_recall(input, init_stage,
				      input_recall_id);
	    }
	    break;
	  }

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* init recall - audio */
	  switch(stage){
	  case 0:
	    {
	      ags_audio_duplicate_recall(audio,
					 audio_recall_id);
	    }
	    break;
	  case 1:
	    {
	      ags_audio_resolve_recall(audio,
				       audio_recall_id);
	    }
	    break;
	  case 2:
	    {
	      ags_audio_init_recall(audio, init_stage,
				    audio_recall_id);
	    }
	    break;
	  }
	}
      }

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* init recall - audio */
      switch(stage){
      case 0:
	{
	  ags_audio_duplicate_recall(audio,
				     audio_recall_id);
	}
	break;
      case 1:
	{
	  ags_audio_resolve_recall(audio,
				   audio_recall_id);
	}
	break;
      case 2:
	{
	  ags_audio_init_recall(audio, init_stage,
				audio_recall_id);
	}
	break;
      }
      
      /* init recall - output */
      switch(stage){
      case 0:
	{
	  ags_channel_duplicate_recall(channel,
				       recall_id);
	}
	break;
      case 1:
	{
	  ags_channel_resolve_recall(channel,
				     recall_id);
	}
	break;
      case 2:
	{
	  ags_channel_init_recall(channel, init_stage,
				  recall_id);
	}
	break;
      }
    }
  }

  void ags_channel_recursive_init_up(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *output;
    AgsChannel *current;
    AgsRecallID *recall_id;
    AgsRecyclingContext *recycling_context;

    guint audio_flags;
    guint audio_channel;
    guint input_line;
	
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *current_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    current = channel;
	
    /* get recycling context */
    pthread_mutex_lock(channel_mutex);
	
    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    if(AGS_IS_OUTPUT(channel)){
      if(!init_down){	    
	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	audio = channel->audio;

	pthread_mutex_unlock(channel_mutex);

	/* get audio mutex */
	pthread_mutex_lock(application_mutex);

	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
	pthread_mutex_unlock(application_mutex);
	  
	/* init of audio */
	pthread_mutex_lock(audio_mutex);

	recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							 recycling_context);

	pthread_mutex_unlock(audio_mutex);

	switch(stage){
	case 0:
	  {
	    ags_audio_duplicate_recall(audio,
				       recall_id);
	  }
	  break;
	case 1:
	  {
	    ags_audio_resolve_recall(audio,
				     recall_id);
	  }
	  break;
	case 2:
	  {
	    ags_audio_init_recall(audio, init_stage,
				  recall_id);
	  }
	  break;
	}
      }
      
      /* jump into loop */
      goto ags_channel_recursive_init_GO_UP;
    }

    /* go up */
    while(current != NULL){
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(current_mutex);

      audio = current->audio;

      audio_channel = current->audio_channel;

      input_line = current->line;

      pthread_mutex_unlock(current_mutex);

      /* init of input */
      pthread_mutex_lock(current_mutex);

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recycling_context);

      pthread_mutex_unlock(current_mutex);

      switch(stage){
      case 0:
	{
	  ags_channel_duplicate_recall(current,
				       recall_id);
	}
	break;
      case 1:
	{
	  ags_channel_resolve_recall(current,
				     recall_id);
	}
	break;
      case 2:
	{
	  ags_channel_init_recall(current, init_stage,
				  recall_id);
	}
	break;
      }
	  
      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
	  
      pthread_mutex_unlock(application_mutex);
	  
      /* init of audio */
      pthread_mutex_lock(audio_mutex);

      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recycling_context);

      pthread_mutex_unlock(audio_mutex);

      switch(stage){
      case 0:
	{
	  ags_audio_duplicate_recall(audio,
				     recall_id);
	}
	break;
      case 1:
	{
	  ags_audio_resolve_recall(audio,
				   recall_id);
	}
	break;
      case 2:
	{
	  ags_audio_init_recall(audio, init_stage,
				recall_id);
	}
	break;
      }
	  
      /* get some fields */
      pthread_mutex_lock(audio_mutex);
	  
      audio_flags = audio->flags;

      output = audio->output;

      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	break;
      }
	  
      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	current = ags_channel_nth(output,
				  audio_channel);
      }else{
	current = ags_channel_nth(output,
				  input_line);
      }

      if(current == NULL){
	break;
      }
	  
    ags_channel_recursive_init_GO_UP:
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* init of output */
      if(current == channel){
	if(!init_down){	    
	  pthread_mutex_lock(current_mutex);

	  recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(current_mutex);

	  switch(stage){
	  case 0:
	    {
	      ags_channel_duplicate_recall(current,
					   recall_id);
	    }
	    break;
	  case 1:
	    {
	      ags_channel_resolve_recall(current,
					 recall_id);
	    }
	    break;
	  case 2:
	    {
	      ags_channel_init_recall(current, init_stage,
				      recall_id);
	    }
	    break;
	  }
	}
      }else{
	pthread_mutex_lock(current_mutex);

	recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							 recycling_context);

	pthread_mutex_unlock(current_mutex);

	switch(stage){
	case 0:
	  {
	    ags_channel_duplicate_recall(current,
					 recall_id);
	  }
	  break;
	case 1:
	  {
	    ags_channel_resolve_recall(current,
				       recall_id);
	  }
	  break;
	case 2:
	  {
	    ags_channel_init_recall(current, init_stage,
				    recall_id);
	  }
	  break;
	}
      }
      
      /* iterate */
      pthread_mutex_lock(current_mutex);
	  
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }
  }
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* check recycling context */  
  if(recall_id->recycling_context == NULL){
    g_critical("recall id without recycling context");
  }

  if(stage == -1){
    for(stage = 0; stage < 3; stage++){
      if(stage < 2){
	/* duplicate and resolve */
	if(init_down){
	  ags_channel_recursive_init_down(channel,
					  recall_id);
	}

	if(init_up){
	  ags_channel_recursive_init_up(channel,
					recall_id);
	}
      }else if(stage == 2){
	if(init_stage == -1){
	  for(init_stage = 0; init_stage < 3; init_stage++){
	    /* init */
	    if(init_down){
	      ags_channel_recursive_init_down(channel,
					      recall_id);
	    }

	    if(init_up){
	      ags_channel_recursive_init_up(channel,
					    recall_id);
	    }
	  }
	}else if(init_stage >= 0 && init_stage < 3){
	  /* init */
	  if(init_down){
	    ags_channel_recursive_init_down(channel,
					    recall_id);
	  }

	  if(init_up){
	    ags_channel_recursive_init_up(channel,
					  recall_id);
	  }
	}
      }
    }
  }else{
    if(stage < 2){
      /* duplicate or resolve */
      if(init_down){
	ags_channel_recursive_init_down(channel,
					recall_id);
      }

      if(init_up){
	ags_channel_recursive_init_up(channel,
				      recall_id);
      }
    }else if(stage == 2){
      if(init_stage == -1){
	for(init_stage = 0; init_stage < 3; init_stage++){
	  /* init */
	  if(init_down){
	    ags_channel_recursive_init_down(channel,
					    recall_id);
	  }

	  if(init_up){
	    ags_channel_recursive_init_up(channel,
					  recall_id);
	  }
	}
      }else if(init_stage >= 0 && init_stage < 3){
	/* init */
	if(init_down){
	  ags_channel_recursive_init_down(channel,
					  recall_id);
	}

	if(init_up){
	  ags_channel_recursive_init_up(channel,
					recall_id);
	}
      }
    }
  }
}

/**
 * ags_channel_recursive_run:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 * @run_up: if %TRUE run going up, else not
 * @run_down: if %TRUE run going down, else not
 * @current_level_only: if %TRUE run only current level, else full recursion
 * 
 * Recursive run the recall id. This means any recall matching @recall_id is
 * going to be ran.
 * 
 * Since: 1.2.7
 */
void
ags_channel_recursive_run(AgsChannel *channel,
			  AgsRecallID *recall_id,
			  gint run_stage,
			  gboolean run_up, gboolean run_down,
			  gboolean current_level_only)
{
  auto void ags_channel_recursive_run_down(AgsChannel *channel, AgsRecallID *recall_id);
  auto void ags_channel_recursive_run_up(AgsChannel *channel, AgsRecallID *recall_id);
  
  void ags_channel_recursive_run_down(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id;
    AgsRecyclingContext *recycling_context;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);
    
    /* special case */
    if(AGS_IS_INPUT(channel)){
      AgsChannel *link;
      AgsRecallID *next_recall_id;
      
      pthread_mutex_t *link_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      link = channel->link;
      
      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	AgsRecycling *link_first_recycling;
	
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);

	/* get link recall id */
	pthread_mutex_lock(link_mutex);
	
	next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
							      recycling_context);
	
	pthread_mutex_unlock(link_mutex);

	/* traverse */
	ags_channel_recursive_reset_recall_id_unset_down(link,
							 next_recall_id);
      }
      
      /* run recall - input */
      ags_channel_play(channel,
		       recall_id,
		       run_stage);

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* run recall - audio */
      ags_audio_play(audio,
		     audio_recall_id,
		     run_stage);
    }else{
      AgsChannel *input;      
      AgsChannel *link;
      AgsRecallID *input_recall_id;
      AgsRecallID *next_recall_id;
      AgsRecyclingContext *next_recycling_context;
      
      guint audio_flags;
      guint audio_channel;
      guint output_line;
      
      pthread_mutex_t *input_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      audio_channel = channel->audio_channel;
      output_line = channel->line;

      pthread_mutex_unlock(channel_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      input = audio->input;
      
      pthread_mutex_unlock(audio_mutex);

      if((current_level_only && (AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) == 0) ||
	 (!current_level_only)){
	if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	  input = ags_channel_nth(input,
				  audio_channel);

	  while(input != NULL){
	    /* get input mutex */
	    pthread_mutex_lock(application_mutex);

	    input_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) input);
  
	    pthread_mutex_unlock(application_mutex);
	  
	    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){	    
	      /* find parent recycling context */
	      pthread_mutex_lock(input_mutex);

	      input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									    recycling_context);
	      next_recycling_context = input_recall_id->recycling_context;
	    
	      pthread_mutex_unlock(input_mutex);	    
	    }else{
	      next_recycling_context = recycling_context;
	    }

	    /* get input recall id and link */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								   next_recycling_context);
	  
	    link = input->link;

	    pthread_mutex_unlock(input_mutex);

	    if(link != NULL){
	      /* get link mutex */
	      pthread_mutex_lock(application_mutex);
	    
	      link_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) link);
	    
	      pthread_mutex_unlock(application_mutex);

	      /* get link recall id */
	      pthread_mutex_lock(link_mutex);
	
	      next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								    next_recycling_context);
	
	      pthread_mutex_unlock(link_mutex);

	      /* traverse */
	      ags_channel_recursive_reset_recall_id_unset_down(link,
							       next_recall_id);
	    }

	    /* run recall - input */
	    ags_channel_play(input,
			     input_recall_id,
			     run_stage);

	    /* get audio recall id */
	    pthread_mutex_lock(audio_mutex);
	
	    audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								   next_recycling_context);
      
	    pthread_mutex_unlock(audio_mutex);
      
	    /* run recall - audio */
	    ags_audio_play(audio,
			   audio_recall_id,
			   run_stage);
	  
	    /* iterate */
	    pthread_mutex_lock(input_mutex);
	  
	    input = input->next;

	    pthread_mutex_unlock(input_mutex);
	  }
	}else{
	  input = ags_channel_nth(input,
				  output_line);

	  if(input != NULL){
	    /* get input mutex */
	    pthread_mutex_lock(application_mutex);

	    input_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) input);
  
	    pthread_mutex_unlock(application_mutex);
	  
	    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	      /* find parent recycling context */
	      pthread_mutex_lock(input_mutex);

	      input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									    recycling_context);
	      next_recycling_context = input_recall_id->recycling_context;
	    
	      pthread_mutex_unlock(input_mutex);	    
	    }else{
	      next_recycling_context = recycling_context;
	    }

	    /* get input recall id and link */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								   next_recycling_context);
	  
	    link = input->link;

	    pthread_mutex_unlock(input_mutex);

	    if(link != NULL){
	      /* get link mutex */
	      pthread_mutex_lock(application_mutex);
	    
	      link_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) link);
	    
	      pthread_mutex_unlock(application_mutex);

	      /* get link recall id */
	      pthread_mutex_lock(link_mutex);
	
	      next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								    next_recycling_context);
	
	      pthread_mutex_unlock(link_mutex);

	      /* traverse */
	      ags_channel_recursive_reset_recall_id_unset_down(link,
							       next_recall_id);
	    }

	    /* run recall - input */
	    ags_channel_play(input,
			     input_recall_id,
			     run_stage);

	    /* get audio recall id */
	    pthread_mutex_lock(audio_mutex);
	
	    audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								   next_recycling_context);
      
	    pthread_mutex_unlock(audio_mutex);
      
	    /* run recall - audio */
	    ags_audio_play(audio,
			   audio_recall_id,
			   run_stage);
	  }
	}
      }
      
      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* run recall - audio */
      ags_audio_play(audio,
		     audio_recall_id,
		     run_stage);
      
      /* run recall - output */
      ags_channel_play(channel,
		       recall_id,
		       run_stage);
    }
  }

  void ags_channel_recursive_run_up(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *output;
    AgsChannel *current;
    AgsRecallID *recall_id;
    AgsRecyclingContext *recycling_context;

    guint audio_flags;
    guint audio_channel;
    guint input_line;
	
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *current_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    current = channel;
	
    /* get recycling context */
    pthread_mutex_lock(channel_mutex);
	
    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    if(AGS_IS_OUTPUT(channel)){
      if(!run_down){	    
	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	audio = channel->audio;

	pthread_mutex_unlock(channel_mutex);

	/* get audio mutex */
	pthread_mutex_lock(application_mutex);

	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
	pthread_mutex_unlock(application_mutex);
	  
	/* run of audio */
	pthread_mutex_lock(audio_mutex);

	recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							 recycling_context);

	pthread_mutex_unlock(audio_mutex);

	ags_audio_play(audio,
		       recall_id,
		       run_stage);
      }
      
      /* jump into loop */
      goto ags_channel_recursive_run_GO_UP;
    }

    /* go up */
    while(current != NULL){
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(current_mutex);

      audio = current->audio;

      audio_channel = current->audio_channel;

      input_line = current->line;

      pthread_mutex_unlock(current_mutex);

      /* run of input */
      pthread_mutex_lock(current_mutex);

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recycling_context);

      pthread_mutex_unlock(current_mutex);

      ags_channel_play(current,
		       recall_id,
		       run_stage);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
	  
      pthread_mutex_unlock(application_mutex);
	  
      /* run of audio */
      pthread_mutex_lock(audio_mutex);

      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recycling_context);

      pthread_mutex_unlock(audio_mutex);

      ags_audio_play(audio,
		     recall_id,
		     run_stage);
	  
      /* get some fields */
      pthread_mutex_lock(audio_mutex);
	  
      audio_flags = audio->flags;

      output = audio->output;

      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	break;
      }
	  
      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	current = ags_channel_nth(output,
				  audio_channel);
      }else{
	current = ags_channel_nth(output,
				  input_line);
      }

      if(current == NULL){
	break;
      }
	  
    ags_channel_recursive_run_GO_UP:
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* run of output */
      if(current == channel){
	if(!run_down){	    
	  pthread_mutex_lock(current_mutex);

	  recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(current_mutex);

	  ags_channel_play(current,
			   recall_id,
			   run_stage);
	}
      }else{
	pthread_mutex_lock(current_mutex);

	recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							 recycling_context);

	pthread_mutex_unlock(current_mutex);

	ags_channel_play(current,
			 recall_id,
			 run_stage);
      }
      
      /* iterate */
      pthread_mutex_lock(current_mutex);
	  
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }
  }
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* check recycling context */  
  if(recall_id->recycling_context == NULL){
    g_critical("recall id without recycling context");
  }

  if(stage == -1){
    for(stage = 0; stage < 3; stage++){
      /* run */
      if(run_down){
	ags_channel_recursive_run_down(channel,
					recall_id);
      }

      if(run_up){
	ags_channel_recursive_run_up(channel,
				      recall_id);
      }
    }
  }else{
    /* run */
    if(run_down){
      ags_channel_recursive_run_down(channel,
				      recall_id);
    }

    if(run_up){
      ags_channel_recursive_run_up(channel,
				    recall_id);
    }
  }
}

/**
 * ags_channel_recursive_cancel:
 * @channel: the #AgsChannel
 * @recall_id: the #AgsRecallID
 * @cancel_up: if %TRUE cancel going up, else not
 * @cancel_down: if %TRUE cancel going down, else not
 * 
 * Recursive cancel the recall id. This means any recall matching @recall_id is
 * going to be canceled.
 * 
 * Since: 1.2.7
 */
void
ags_channel_recursive_cancel(AgsChannel *channel,
			     AgsRecallID *recall_id,
			     gboolean cancel_up, gboolean cancel_down)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  auto void ags_channel_recursive_cancel_down(AgsChannel *channel, AgsRecallID *recall_id);
  auto void ags_channel_recursive_cancel_up(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_cancel_down(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id;
    AgsRecyclingContext *recycling_context;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);
    
    /* special case */
    if(AGS_IS_INPUT(channel)){
      AgsChannel *link;
      AgsRecallID *next_recall_id;
      
      pthread_mutex_t *link_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      link = channel->link;
      
      pthread_mutex_unlock(channel_mutex);

      if(link != NULL){
	AgsRecycling *link_first_recycling;
	
	/* get link mutex */
	pthread_mutex_lock(application_mutex);

	link_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) link);
  
	pthread_mutex_unlock(application_mutex);

	/* get link recall id */
	pthread_mutex_lock(link_mutex);
	
	next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
							      recycling_context);
	
	pthread_mutex_unlock(link_mutex);

	/* traverse */
	ags_channel_recursive_reset_recall_id_unset_down(link,
							 next_recall_id);
      }
      
      /* cancel recall - input */
      ags_channel_cancel_recall(channel,
				recall_id);

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* cancel recall - audio */
      ags_audio_cancel_recall(audio,
			      audio_recall_id);
    }else{
      AgsChannel *input;      
      AgsChannel *link;
      AgsRecallID *input_recall_id;
      AgsRecallID *next_recall_id;
      AgsRecyclingContext *next_recycling_context;
      
      guint audio_flags;
      guint audio_channel;
      guint output_line;
      
      pthread_mutex_t *input_mutex;

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      audio_channel = channel->audio_channel;
      output_line = channel->line;

      pthread_mutex_unlock(channel_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_flags = audio->flags;
      
      input = audio->input;
      
      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	input = ags_channel_nth(input,
				audio_channel);

	while(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* cancel recall - input */
	  ags_channel_cancel_recall(input,
				    input_recall_id);

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* cancel recall - audio */
	  ags_audio_cancel_recall(audio,
				  audio_recall_id);
	  
	  /* iterate */
	  pthread_mutex_lock(input_mutex);
	  
	  input = input->next;

	  pthread_mutex_unlock(input_mutex);
	}
      }else{
	input = ags_channel_nth(input,
				output_line);

	if(input != NULL){
	  /* get input mutex */
	  pthread_mutex_lock(application_mutex);

	  input_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) input);
  
	  pthread_mutex_unlock(application_mutex);
	  
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	    /* find parent recycling context */
	    pthread_mutex_lock(input_mutex);

	    input_recall_id = ags_recall_id_find_parent_recycling_context(input->recall_id,
									  recycling_context);
	    next_recycling_context = input_recall_id->recycling_context;
	    
	    pthread_mutex_unlock(input_mutex);	    
	  }else{
	    next_recycling_context = recycling_context;
	  }

	  /* get input recall id and link */
	  pthread_mutex_lock(input_mutex);

	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 next_recycling_context);
	  
	  link = input->link;

	  pthread_mutex_unlock(input_mutex);

	  if(link != NULL){
	    /* get link mutex */
	    pthread_mutex_lock(application_mutex);
	    
	    link_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link);
	    
	    pthread_mutex_unlock(application_mutex);

	    /* get link recall id */
	    pthread_mutex_lock(link_mutex);
	
	    next_recall_id = ags_recall_id_find_recycling_context(link->recall_id,
								  next_recycling_context);
	
	    pthread_mutex_unlock(link_mutex);

	    /* traverse */
	    ags_channel_recursive_reset_recall_id_unset_down(link,
							     next_recall_id);
	  }

	  /* cancel recall - input */
	  ags_channel_cancel_recall(input,
				    input_recall_id);

	  /* get audio recall id */
	  pthread_mutex_lock(audio_mutex);
	
	  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
								 next_recycling_context);
      
	  pthread_mutex_unlock(audio_mutex);
      
	  /* cancel recall - audio */
	  ags_audio_cancel_recall(audio,
				  audio_recall_id);
	}
      }

      /* get audio recall id */
      pthread_mutex_lock(audio_mutex);
	
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
      
      pthread_mutex_unlock(audio_mutex);
      
      /* cancel recall - audio */
      ags_audio_cancel_recall(audio,
			      audio_recall_id);
      
      /* cancel recall - output */
      ags_channel_cancel_recall(channel,
				recall_id);
    }
  }

  void ags_channel_recursive_cancel_up(AgsChannel *channel, AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *output;
    AgsChannel *current;
    AgsRecallID *recall_id;
    AgsRecyclingContext *recycling_context;

    guint audio_flags;
    guint audio_channel;
    guint input_line;
	
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *current_mutex;

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    current = channel;
	
    /* get recycling context */
    pthread_mutex_lock(channel_mutex);
	
    recycling_context = recall_id->recycling_context;

    pthread_mutex_unlock(channel_mutex);

    if(AGS_IS_OUTPUT(channel)){
      if(!cancel_down){	    
	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	audio = channel->audio;

	pthread_mutex_unlock(channel_mutex);

	/* get audio mutex */
	pthread_mutex_lock(application_mutex);

	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
	pthread_mutex_unlock(application_mutex);
	  
	/* cancel of audio */
	pthread_mutex_lock(audio_mutex);

	recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							 recycling_context);

	pthread_mutex_unlock(audio_mutex);

	ags_audio_cancel_recall(audio,
				recall_id);
      }
      
      /* jump into loop */
      goto ags_channel_recursive_cancel_GO_UP;
    }

    /* go up */
    while(current != NULL){
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(current_mutex);

      audio = current->audio;

      audio_channel = current->audio_channel;

      input_line = current->line;

      pthread_mutex_unlock(current_mutex);

      /* cancel of input */
      pthread_mutex_lock(current_mutex);

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recycling_context);

      pthread_mutex_unlock(current_mutex);

      ags_channel_cancel_recall(current,
				recall_id);
	  
      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
	  
      pthread_mutex_unlock(application_mutex);
	  
      /* cancel of audio */
      pthread_mutex_lock(audio_mutex);

      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recycling_context);

      pthread_mutex_unlock(audio_mutex);
      
      ags_audio_cancel_recall(audio,
			      recall_id);
	  
      /* get some fields */
      pthread_mutex_lock(audio_mutex);
	  
      audio_flags = audio->flags;

      output = audio->output;

      pthread_mutex_unlock(audio_mutex);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & audio_flags) != 0){
	break;
      }
	  
      if((AGS_AUDIO_ASYNC & audio_flags) != 0){
	current = ags_channel_nth(output,
				  audio_channel);
      }else{
	current = ags_channel_nth(output,
				  input_line);
      }

      if(current == NULL){
	break;
      }
	  
    ags_channel_recursive_cancel_GO_UP:
      /* get current mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* cancel of output */
      if(current == channel){
	if(!cancel_down){	    
	  pthread_mutex_lock(current_mutex);

	  recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							   recycling_context);

	  pthread_mutex_unlock(current_mutex);

	  ags_channel_cancel_recall(current,
				    recall_id);
	}
      }else{
	pthread_mutex_lock(current_mutex);

	recall_id = ags_recall_id_find_recycling_context(current->recall_id,
							 recycling_context);

	pthread_mutex_unlock(current_mutex);

	ags_channel_cancel_recall(current,
				  recall_id);
      }
      
      /* iterate */
      pthread_mutex_lock(current_mutex);
	  
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }
  }
  
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* check recycling context */
  if(recall_id->recycling_context == NULL){
    g_critical("recall id without recycling context");
  }
  
  if(cancel_down){
    ags_channel_recursive_cancel_down(channel,
				      recall_id);
  }

  if(cancel_up){
    ags_channel_recursive_cancel_up(channel,
				    recall_id);    
  }
}

/**
 * ags_channel_new:
 * @audio: the #AgsAudio
 *
 * Creates a #AgsChannel, linking tree to @audio.
 *
 * Returns: a new #AgsChannel
 *
 * Since: 1.0.0
 */
AgsChannel*
ags_channel_new(GObject *audio)
{
  AgsChannel *channel;

  channel = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
					"audio", audio,
					NULL);

  return(channel);
}
