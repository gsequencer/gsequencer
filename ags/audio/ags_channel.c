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

#include <ags/object/ags_config.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_concurrent_tree.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_registry.h>

#include <ags/file/ags_file_link.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
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

#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_recycling_thread.h>

#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_remove_recall.h>
#include <ags/audio/task/ags_remove_recall_container.h>

#include <ags/audio/file/ags_audio_file.h>

#include <stdio.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

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
static void ags_channel_finalize(GObject *gobject);

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
  PROP_PAD,
  PROP_AUDIO_CHANNEL,
  PROP_LINE,
  PROP_NOTE,
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
					      "AgsChannel\0",
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

  gobject->finalize = ags_channel_finalize;

  /* properties */
  /**
   * AgsChannel:audio:
   *
   * The assigned #AgsAudio aligning channels.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio it is assigned with\0",
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
   * AgsAudio:audio-channel:
   *
   * The nth audio channel.
   * 
   * Since: 0.7.2
   */
  param_spec =  g_param_spec_uint("audio-channel\0",
				  "nth audio channel\0",
				  "The nth audio channel\0",
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
   * Since: 0.7.2
   */
  param_spec =  g_param_spec_uint("pad\0",
				  "nth pad\0",
				  "The nth pad\0",
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
   * Since: 0.7.2
   */
  param_spec =  g_param_spec_uint("line\0",
				  "nth line\0",
				  "The nth line\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_string("note\0",
				   "assigned note\0",
				   "The note it is assigned with\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("playback\0",
				   "assigned playback\0",
				   "The playback it is assigned with\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("recall-id\0",
				   "assigned recall id\0",
				   "The recall id it is assigned with\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsChannel:recall-container:
   *
   * The containing #AgsRecallContainer.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("recall-container\0",
				   "containing recall-container\0",
				   "The recall container it contains\0",
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsChannel:recall:
   *
   * The containing #AgsRecall in recall-context.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("recall\0",
				   "containing recall\0",
				   "The recall it contains\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);

  /**
   * AgsChannel:play:
   *
   * The containing #AgsRecall in play-context.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("play\0",
				   "containing play\0",
				   "The play it contains\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /**
   * AgsChannel:link:
   *
   * The assigned link as #AgsChannel.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("link\0",
				   "assigned link\0",
				   "The link it is assigned with\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("first-recycling\0",
				   "containing first recycling\0",
				   "The first recycling it contains\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("last-recycling\0",
				   "containing last recycling\0",
				   "The last recycling it contains\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("pattern\0",
				   "containing pattern\0",
				   "The pattern it contains\0",
				   AGS_TYPE_PATTERN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);
  
  /* AgsChannelClass */
  channel->add_effect = ags_channel_real_add_effect;
  channel->remove_effect = ags_channel_real_remove_effect;

  channel->recycling_changed = NULL;

  channel->done = NULL;

  /* signals */
    /**
   * AgsChannel::add-effect:
   * @channel: the #AgsChannel to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   */
  channel_signals[ADD_EFFECT] =
    g_signal_new("add-effect\0",
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
    g_signal_new("remove-effect\0",
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
    g_signal_new("recycling-changed\0",
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
    g_signal_new("done\0",
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
  return(g_quark_from_static_string("ags-channel-error-quark\0"));
}

void
ags_channel_init(AgsChannel *channel)
{
  AgsConfig *config;
  AgsMutexManager *mutex_manager;

  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  /* create mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  /* insert mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) channel,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  channel->flags = 0;

  channel->audio = NULL;
  channel->soundcard = NULL;

  channel->prev = NULL;
  channel->prev_pad = NULL;
  channel->next = NULL;
  channel->next_pad = NULL;

  channel->pad = 0;
  channel->audio_channel = 0;
  channel->line = 0;

  channel->note = NULL;

  channel->playback = ags_playback_new();
  AGS_PLAYBACK(channel->playback)->source = (GObject *) channel;

  /* config */
  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model\0");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope\0");
  
  if(!g_ascii_strncasecmp(str0,
			  "super-threaded\0",
			  15)){
    if(!g_ascii_strncasecmp(str1,
			    "channel\0",
			    8) ||
       !g_ascii_strncasecmp(str1,
			    "recycling\0",
			    10)){
      g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
		      AGS_PLAYBACK_SUPER_THREADED_CHANNEL);

      AGS_PLAYBACK(channel->playback)->channel_thread[0] = ags_channel_thread_new(NULL,
										  channel);
      AGS_PLAYBACK(channel->playback)->channel_thread[1] = ags_channel_thread_new(NULL,
										  channel);
      AGS_PLAYBACK(channel->playback)->channel_thread[2] = ags_channel_thread_new(NULL,
										  channel);
      
      if(!g_ascii_strncasecmp(str1,
			      "recycling\0",
			      10)){
	AGS_PLAYBACK(channel->playback)->recycling_thread[0] = ags_recycling_thread_new(NULL,
											NULL);
	AGS_PLAYBACK(channel->playback)->recycling_thread[1] = ags_recycling_thread_new(NULL,
											NULL);
	AGS_PLAYBACK(channel->playback)->recycling_thread[2] = ags_recycling_thread_new(NULL,
											NULL);
      }
    }
  }
 
  free(str0);
  free(str1);
   
  channel->recall_id = NULL;
  channel->container = NULL;

  channel->recall = NULL;
  channel->play = NULL;

  channel->link = NULL;
  
  channel->first_recycling = NULL;
  channel->last_recycling = NULL;

  channel->pattern = NULL;
  channel->notation = NULL;

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

      if(channel->playback == playback){
	return;
      }

      if(channel->playback != NULL){
	g_object_unref(channel->playback);
      }

      if(playback != NULL){
	g_object_ref(playback);
      }

      channel->playback = playback;
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

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

      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      if(recall_container == NULL ||
	 g_list_find(channel->container, recall_container) != NULL){
	return;
      }

      ags_channel_add_recall_container(channel,
				       recall_container);
    }
    break;
  case PROP_RECALL:
    {
      AgsRecall *recall;

      recall = (AgsRecall *) g_value_get_object(value);

      if(recall == NULL ||
	 g_list_find(channel->recall, recall) != NULL){
	return;
      }

      ags_channel_add_recall(channel,
			     recall,
			     FALSE);
    }
    break;
  case PROP_PLAY:
    {
      AgsRecall *play;

      play = (AgsRecall *) g_value_get_object(value);

      if(play == NULL ||
	 g_list_find(channel->play, play) != NULL){
	return;
      }

      ags_channel_add_recall(channel,
			     play,
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
      AgsChannel *pattern;

      pattern = (AgsRecall *) g_value_get_object(value);

      if(pattern == NULL ||
	 g_list_find(channel->pattern, pattern) != NULL){
	return;
      }

      ags_channel_add_pattern(channel,
			      pattern);
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
    g_value_set_object(value, channel->audio);
    break;
  case PROP_SOUNDCARD:
    g_value_set_object(value, channel->soundcard);
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
      g_value_set_pointer(value, g_list_copy(channel->recall));
    }
    break;
  case PROP_PLAY:
    {
      g_value_set_pointer(value, g_list_copy(channel->play));
    }
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

  AgsServer *server;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;
  
  channel = AGS_CHANNEL(connectable);

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(channel->soundcard));

  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

  entry = ags_registry_entry_alloc(server->registry);
  g_value_set_object(&(entry->entry),
		     (gpointer) channel);
  ags_registry_add(server->registry,
		   entry);

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

#ifdef AGS_DEBUG
  g_message("connecting channel\0");
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

  if(channel->notation != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(channel->notation));
  }
}

void
ags_channel_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
  /* empty */
}

static void
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

  //FIXME:JK: wrong ref count
  if(channel->audio != NULL){
    //    g_object_unref(channel->audio);
  }
  
  /* AgsRecycling */
  if(((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_INPUT(channel)) ||
     ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_OUTPUT(channel))){

    recycling = channel->first_recycling;

    if(recycling != NULL){
      while(recycling != channel->last_recycling->next){
	recycling_next = recycling->next;

	g_object_unref((GObject *) recycling);

	recycling = recycling_next;
      }
    }
  }

  /* key string */
  if(channel->note != NULL){
    free(channel->note);
  }

  if(channel->playback != NULL){
    g_object_unref(channel->playback);  
  }

  /* free some lists */
  g_list_free_full(channel->recall_id,
		   g_object_unref);
  g_list_free_full(channel->container,
		   g_object_unref);
  
  g_list_free_full(channel->recall,
		   g_object_unref);
  g_list_free_full(channel->play,
		   g_object_unref);

  g_list_free_full(channel->pattern,
		   g_object_unref);

  /* AgsNotation */
  if(channel->notation != NULL){
    g_object_unref(channel->notation);
  }
  
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
 * Since: 0.3
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
 * Since: 0.3
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
 * Since: 0.3
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

  if((nth != 0 && i != nth) || channel == NULL){
    g_message("ags_channel_nth:\n  nth channel does not exist\n  `- stopped @: i = %u; nth = %u\n\0", i, nth);
  }
  
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
 * Since: 0.3
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
 * Since: 0.3
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
 * Since: 0.3
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

  if((nth != 0 && i != nth) || channel == NULL){
    g_message("ags_channel_nth_pad:\n  nth pad does not exist\n  `- stopped @: i = %u; nth = %u\0", i, nth);
  }
  
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
 * Since: 0.3
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
 * Since: 0.3
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
 * Since: 0.4
 */
void
ags_channel_set_soundcard(AgsChannel *channel, GObject *soundcard)
{
  AgsMutexManager *mutex_manager;

  GList *list;

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

  /* playback */
  if(AGS_PLAYBACK(channel->playback)->channel_thread[0] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[0],
		 "soundcard\0", soundcard,
		 NULL);
  }
    
  if(AGS_PLAYBACK(channel->playback)->channel_thread[1] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[1],
		 "soundcard\0", soundcard,
		 NULL);
  }
  
  if(AGS_PLAYBACK(channel->playback)->channel_thread[2] != NULL){
    g_object_set(AGS_PLAYBACK(channel->playback)->channel_thread[2],
		 "soundcard\0", soundcard,
		 NULL);
  }
  
  /* recall */
  list = channel->play;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard\0", soundcard,
		 NULL);

    list = list->next;
  }

  list = channel->recall;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard\0", soundcard,
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
		     "soundcard\0", soundcard,
		     NULL); 

	recycling = recycling->next;
      }
    }
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_add_recall_id:
 * @channel: an #AgsChannel
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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
 * ags_channel_remove_recall:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Removes a recall.
 *
 * Since: 0.4
 */
void
ags_channel_remove_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* add recall */
  pthread_mutex_lock(mutex);

  if(play){
    channel->play = g_list_remove(channel->play, recall);
  }else{
    channel->recall = g_list_remove(channel->recall, recall);
  }

  g_object_unref(G_OBJECT(recall));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_add_recall:
 * @channel: an #AgsChannel
 * @recall_container: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Adds a recall.
 *
 * Since: 0.4
 */
void
ags_channel_add_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL || recall == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* remove recall */
  pthread_mutex_lock(mutex);

  g_object_ref(recall);

  if(play){
    channel->play = g_list_prepend(channel->play, recall);
  }else{
    channel->recall = g_list_prepend(channel->recall, recall);
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_remove_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 0.7.2
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

/**
 * ags_channel_remove_pattern:
 * @channel: an #AgsChannel
 * @pattern: the #AgsPattern
 *
 * Removes a pattern.
 *
 * Since: 0.7.2
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
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLadspa *recall_ladspa;
  AgsLadspaPlugin *ladspa_plugin;
  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;
  
  GList *port;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  long effect_index;

  /* load plugin */
  ags_ladspa_manager_load_file(filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename, effect);

  effect_index = ags_ladspa_manager_effect_index(filename,
						 effect);

  /* ladspa play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);

  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);
  g_object_set(G_OBJECT(recall_ladspa),
	       "soundcard\0", channel->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_ladspa,
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
	       "soundcard\0", channel->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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

  /* ladspa recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);
  
  recall_ladspa = ags_recall_ladspa_new(channel,
					filename,
					effect,
					effect_index);
  g_object_set(G_OBJECT(recall_ladspa),
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_ladspa,
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
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_ladspa,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 FALSE);  
  
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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

  return(port);
}

GList*
ags_channel_add_dssi_effect(AgsChannel *channel,
			    gchar *filename,
			    gchar *effect)
{
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallDssi *recall_dssi;
  AgsDssiPlugin *dssi_plugin;
  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;
  
  GList *port;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  long effect_index;

  /* load plugin */
  ags_dssi_manager_load_file(filename);
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(filename, effect);

  effect_index = ags_dssi_manager_effect_index(filename,
					       effect);

  /* dssi play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);

  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  g_object_set(G_OBJECT(recall_dssi),
	       "soundcard\0", channel->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  AGS_RECALL(recall_dssi)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_dssi,
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
	       "soundcard\0", channel->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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

  /* dssi recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);
  
  recall_dssi = ags_recall_dssi_new(channel,
				    filename,
				    effect,
				    effect_index);
  g_object_set(G_OBJECT(recall_dssi),
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_dssi,
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
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_dssi,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 FALSE);  
  
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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

  return(port);
}

GList*
ags_channel_add_lv2_effect(AgsChannel *channel,
			   gchar *filename,
			   gchar *effect)
{
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLv2 *recall_lv2;
  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;
  
  AgsLv2Plugin *lv2_plugin;

  GList *uri_node;
  GList *port;

  gchar *uri;
  gchar *str;
  
  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;
  uint32_t effect_index;

  /* find plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename, effect);

  uri = ags_lv2_manager_find_uri(filename,
				 effect);
  
  effect_index = ags_lv2_manager_uri_index(filename,
					   uri);
  
  /* lv2 play */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);

  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  g_object_set(G_OBJECT(recall_lv2),
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_lv2,
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
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 TRUE);
  
  /* check if connected or running */
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;
    
    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent == NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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

  /* lv2 recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(channel,
				   recall_container);
  
  recall_lv2 = ags_recall_lv2_new(channel,
				  lv2_plugin->turtle,
				  filename,
				  effect,
				  uri,
				  effect_index);
  g_object_set(G_OBJECT(recall_lv2),
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_lv2,
			 FALSE);
  
  AGS_RECALL(recall_lv2)->flags |= AGS_RECALL_TEMPLATE;
  ags_recall_lv2_load(recall_lv2);

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
	       "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
	       "recall-container\0", recall_container,
	       "recall-channel\0", recall_lv2,
	       NULL);
  ags_channel_add_recall(channel,
			 recall_channel_run_dummy,
			 FALSE);  
  
  if((AGS_CHANNEL_CONNECTED & (channel->flags)) != 0){
    AgsRecall *current;
    GList *recall_id;

    ags_connectable_connect(AGS_CONNECTABLE(recall_container));
    ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));
    ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

    recall_id = channel->recall_id;
    
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context->parent != NULL){
	current = ags_recall_duplicate(recall_channel_run_dummy, recall_id->data);

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
  
  GList *port;

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_channel_add_ladspa_effect(channel,
					 filename,
					 effect);
  }

  if(ladspa_plugin == NULL){
    dssi_plugin = ags_dssi_manager_find_dssi_plugin(filename, effect);
    
    if(dssi_plugin != NULL){
      port = ags_channel_add_dssi_effect(channel,
					 filename,
					 effect);
    }
  }
  
  if(ladspa_plugin == NULL &&
     dssi_plugin == NULL){
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_channel_add_lv2_effect(channel,
					filename,
					effect);
    }
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
  AgsRemoveRecallContainer *remove_recall_container;
  AgsRemoveRecall *remove_recall;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  GList *play_ladspa, *recall_ladspa;
  GList *task;

  application_context = ags_soundcard_get_application_context(AGS_AUDIO(channel->audio)->soundcard);
  main_loop = application_context->main_loop;
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  play_ladspa = ags_recall_template_find_type(channel->play,
					      AGS_TYPE_RECALL_LADSPA);
  
  recall_ladspa = ags_recall_template_find_type(channel->recall,
						AGS_TYPE_RECALL_LADSPA);
  
  task = NULL;

  /* play context */
  remove_recall = ags_remove_recall_new(channel,
					g_list_nth(play_ladspa,
						   nth)->data,
					TRUE,
					TRUE);
  task = g_list_prepend(task,
			remove_recall);

  remove_recall = ags_remove_recall_new(channel,
					ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(g_list_nth(play_ladspa,
													    nth)->data)->container)->recall_channel_run)->data,
					TRUE,
					TRUE);
  task = g_list_prepend(task,
			remove_recall);

  remove_recall_container = ags_remove_recall_container_new(channel->audio,
							    AGS_RECALL(g_list_nth(play_ladspa,
										  nth)->data)->container);
  task = g_list_prepend(task,
			remove_recall_container);

  /* recall context */
  remove_recall = ags_remove_recall_new(channel,
					g_list_nth(recall_ladspa,
						   nth)->data,
					FALSE,
					TRUE);
  task = g_list_prepend(task,
			remove_recall);

  remove_recall = ags_remove_recall_new(channel,
					ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(g_list_nth(recall_ladspa,
													    nth)->data)->container)->recall_channel_run)->data,
					FALSE,
					TRUE);
  task = g_list_prepend(task,
			remove_recall);

  remove_recall_container = ags_remove_recall_container_new(channel->audio,
							    AGS_RECALL(g_list_nth(recall_ladspa,
										  nth)->data)->container);
  task = g_list_prepend(task,
			remove_recall_container);

  /* launch task */
  ags_task_thread_append_tasks(task_thread,
			       task);
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
 * Since: 0.4
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
 * Since: 0.3
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
  g_message("duplicate channel %d\0", channel->line);
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
      //      g_message("%x - %x\0", recall->flags, recall_id->flags);
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
    g_message("recall duplicated: %s\0", G_OBJECT_TYPE_NAME(copy));
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
 * Since: 0.4
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
  g_message("resolve channel %d\0", channel->line);
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
 * @recall_id: the #AgsRecallId this recall belongs to
 *
 * Prepare #AgsRecall objects to become runnning, ags_channel_recursive_play_init()
 * may call this function for you.
 *
 * Since: 0.3
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
  g_message("ags_channel_init_recall@%d - audio::IN[%u]; channel: %llu %llu\n\0",
	    stage,
	    AGS_AUDIO(channel->audio)->input_lines,
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

	parent_container = ags_recall_id_find_parent_recycling_context(AGS_AUDIO(channel->audio)->recall_id,
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
      g_message("recall run init: %s - %x\0", G_OBJECT_TYPE_NAME(recall), recall->flags);
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
 * Since: 0.3
 */
void
ags_channel_play(AgsChannel *channel,
		 AgsRecallID *recall_id,
		 gint stage)
{
  AgsRecall *recall;
  GList *list, *list_next;

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

  /* play or recall */
  pthread_mutex_lock(mutex);

  if(recall_id->recycling_context->parent != NULL){
    list = channel->recall;
  }else{
    list = channel->play;
  }

  /* run */
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if(recall == NULL){
      if(recall_id->recycling_context->parent != NULL){
	channel->recall = g_list_remove(channel->recall,
					recall);
      }else{
	channel->play = g_list_remove(channel->play,
				      recall);
      }

      g_warning("recall == NULL\0");
      list = list_next;
      continue;
    }

    if(recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context ||
       AGS_IS_RECALL_CHANNEL(recall)){
      list = list_next;
      continue;
    }

    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
#ifdef AGS_DEBUG
      g_message("%s play channel %x:%d @%x -> %x\0", G_OBJECT_TYPE_NAME(recall), channel, channel->line, recall, recall->recall_id);
#endif

      if((AGS_RECALL_HIDE & (recall->flags)) == 0){
	if(stage == 0){
	  ags_recall_run_pre(recall);
	}else if(stage == 1){
	  ags_recall_run_inter(recall);
	}else{
	  ags_recall_run_post(recall);
	}
      }
    }

    list = list_next;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_channel_done:
 * @channel: an #AgsChannel
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as playing channel is done.
 *
 * Since: 0.4
 */
void
ags_channel_done(AgsChannel *channel,
		 AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(channel == NULL){
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

  if(!(AGS_IS_CHANNEL(channel))){
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
 * Since: 0.3
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
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;
      continue;
    }

    g_object_ref(recall_id);
    g_object_ref(recall);
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
 * Since: 0.3
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
			      recall,
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
 * Since: 0.4
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
 * your machine running. Thus it adds #AgsRecallID. Asynchronously only.
 *
 * Since: 0.3
 */
void
ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
		     GError **error)
{
  AgsChannel *old_channel_link, *old_link_link;
  AgsRecycling *first_recycling, *last_recycling;

  GError *this_error;

  if(channel == NULL &&
     link == NULL){
    return;
  }

  if(channel != NULL){
    if(channel->link == link){
      return;
    }

    old_channel_link = channel->link;
  }else{
    if(link->link == NULL){
      return;
    }

    old_channel_link = NULL;
  }

  if(link != NULL){
    old_link_link = link->link;
  }else{
    old_link_link = NULL;
  }
  
  g_message("set link %x %x\0", channel, link);

  if(link != NULL){
    old_link_link = link->link;
  }else{
    old_link_link = NULL;
  }

  /* check for a loop */
  if(channel != NULL &&
     link != NULL){
    AgsAudio *audio, *current_audio;
    AgsChannel *current_channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(link->audio);
      current_channel = link;
    }else{
      audio = AGS_AUDIO(channel->audio);
      current_channel = channel;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current_channel = ags_channel_nth(audio->output, link->audio_channel);
    }else{
      current_channel = ags_channel_nth(audio->output, link->line);
    }

    if(current_channel != NULL){
      current_channel = current_channel->link;
    }

    if(current_channel != NULL){
      current_audio = AGS_AUDIO(current_channel->audio);

      while(current_channel != NULL){
	if(current_audio == audio){
	  if(error != NULL){
	    g_set_error(error,
			AGS_CHANNEL_ERROR,
			AGS_CHANNEL_ERROR_LOOP_IN_LINK,
			"failed to link channel %u from %s with channel %u from %s\0",
			channel->line, G_OBJECT_TYPE_NAME(audio),
			link->line, G_OBJECT_TYPE_NAME(link->audio));
	  }

	  return;
	}

	if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
	  current_channel = ags_channel_nth(current_audio->output, current_channel->audio_channel);
	}else{
	  current_channel = ags_channel_nth(current_audio->output, current_channel->line);
	}

	if(current_channel != NULL){
	  current_channel = current_channel->link;
	}	  

	if(current_channel != NULL){
	  current_audio = AGS_AUDIO(current_channel->audio);
	}
      }
    }
  }

  /* set link */
  if(channel != NULL){
    channel->link = link;
  }  

  if(link != NULL){
    link->link = channel;

    if(channel != NULL){
      ags_channel_set_link(link,
			   channel,
			   &this_error);
    }
  }

  /* set old input/output to NULL */
  if(old_link_link != NULL){
    old_link_link->link = NULL;
    
    this_error = NULL;

    ags_channel_set_link(old_link_link,
			 NULL,
			 &this_error);
  }

  /* set old input/output to NULL */
  if(old_channel_link != NULL){
    old_channel_link->link = NULL;
      
    this_error = NULL;
    
    ags_channel_set_link(old_channel_link,
			 NULL,
			 &this_error);
  }

  /* set recycling */
  if(channel != NULL){    
    if(AGS_IS_OUTPUT(channel)){
      first_recycling = channel->first_recycling;
      last_recycling = channel->last_recycling;

      if(link != NULL){
	ags_channel_set_recycling(link,
				  first_recycling, last_recycling,
				  TRUE, TRUE);
      }

      if(AGS_IS_INPUT(old_channel_link)){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(old_channel_link->audio)->flags)) != 0){
	  first_recycling = g_object_new(AGS_TYPE_RECYCLING,
					 "channel\0", channel,
					 "soundcard\0", AGS_AUDIO(channel->audio)->soundcard,
					 NULL);
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
	first_recycling = link->first_recycling;
	last_recycling = link->last_recycling;

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
					 "channel\0", channel,
					 "soundcard\0", channel->soundcard,
					 NULL);
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
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(link->audio)->flags)) != 0){
	first_recycling = g_object_new(AGS_TYPE_RECYCLING,
				       "channel\0", link,
				       "soundcard\0", link->soundcard,
				       NULL);
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
 * Asynchronously only.
 *
 * Since: 0.3
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
  AgsRecyclingContext *recycling_context, *old_recycling_context;
  AgsRecallID *current_recall_id;

  GList *recall_id;

  guint flags;
  guint complete_level_first, complete_level_last;
  gboolean is_output;
  gboolean replace_first, replace_last;
  gboolean find_prev, find_next;
  gboolean change_old_last, change_old_first;

  auto gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input);
  auto void ags_channel_set_recycling_recursive_output(AgsChannel *output);
  auto void ags_channel_set_recycling_recursive(AgsChannel *input);

  auto void ags_channel_set_recycling_emit_changed_input(AgsChannel *input);
  auto void ags_channel_set_recycling_emit_changed_output(AgsChannel *output);
  auto void ags_channel_set_recycling_emit_changed(AgsChannel *input);
 
  gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input){
    AgsChannel *nth_channel_prev, *nth_channel_next;

    if(input == NULL ||
       (input->first_recycling == replace_with_first_recycling &&
	input->last_recycling == replace_with_last_recycling)){
      return(TRUE);
    }

    /* set recycling */
    if(replace_first){
      input->first_recycling = replace_with_first_recycling;
    }
    if(replace_last){
      input->last_recycling = replace_with_last_recycling;
    }
    
    /* search for neighboor recyclings */
    if((AGS_AUDIO_ASYNC & (AGS_AUDIO(input->audio)->flags)) != 0){
      if(find_prev){
	nth_channel_prev = ags_channel_prev_with_recycling(input);

	if(nth_channel_prev != NULL){
#ifdef AGS_DEBUG
	  g_message("found prev\0");
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
	  g_message("found next\0");
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
	    replace_with_last_recycling = prev_channel->last_recycling;

	    find_next = FALSE;
	  }
	}else{
	  replace_with_last_recycling = prev_channel->last_recycling;
	  replace_with_first_recycling = next_channel->first_recycling;
	}
      }else{
	if(next_channel != NULL){
	  if(replace_with_first_recycling == NULL){
	    replace_with_first_recycling = next_channel->first_recycling;

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
    
    if(output == NULL){
      return;
    }

    /* get audio and audio channel */
    audio = AGS_AUDIO(output->audio);

    audio_channel = output->audio_channel;

    /* get input */    
    input = audio->input;
    flags = audio->flags;

    /* update input AgsRecallIDs */
    //TODO:JK: check for compliance
    //    ags_recall_id_reset_recycling(output->recall_id,
    //				  output->first_recycling,
    //				  replace_with_first_recycling, replace_with_last_recycling);

    if(replace_last){      
      /* do it so */
      output->last_recycling = replace_with_last_recycling;
    }

    /* last recycling */
    if(replace_first){
      /* do it so */
      output->first_recycling = replace_with_first_recycling;
    }

    /* deeper level */
    link = output->link;
    
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

    if(input == NULL){
      return;
    }

    /* get audio and audio channel */    
    audio = AGS_AUDIO(input->audio);

    audio_channel = input->audio_channel;
    line = input->line;

    /* get output */
    output = audio->output;
    flags = audio->flags;
        
    /* AgsInput */
    completed = ags_channel_set_recycling_recursive_input(input);

    if(completed){
      return;
    }
    
    /* AgsOutput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	output = ags_channel_nth(output, audio_channel);
      }else{
	output = ags_channel_nth(output, line);
      }
      
      ags_channel_set_recycling_recursive_output(output);
    }
  }
  
  void ags_channel_set_recycling_emit_changed_input(AgsChannel *input){
    AgsRecycling *input_first_recycling, *input_last_recycling;
    
    if(input == NULL){
      return;
    }

    /* get current recycling */
    input_first_recycling = input->first_recycling;
    input_last_recycling = input->last_recycling;
    
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
    
    if(output == NULL){
      return;
    }

    /* get current recycling */
    link = output->link;

    output_first_recycling = output->first_recycling;
    output_last_recycling = output->last_recycling;
      
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

    if(input == NULL){
      return;
    }

    /* get audio and audio channel */
    audio = AGS_AUDIO(input->audio);

    audio_channel = input->audio_channel;
    line = input->line;

    /* get output */
    output = audio->output;
    flags = audio->flags;
    
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
    
  audio = AGS_AUDIO(channel->audio);
  
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

  channel->first_recycling = first_recycling;
  channel->last_recycling = last_recycling;

  /* join now the retrieved recyclings */
  if(!(AGS_IS_INPUT(channel) &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)){
    if(first_recycling != NULL){
      if((prev_channel != NULL &&
	  prev_channel->last_recycling != NULL)){
	prev_channel->last_recycling->next = first_recycling;
	first_recycling->prev = prev_channel->last_recycling;
      }else{
	first_recycling->prev = NULL;
      }
    
      if(next_channel != NULL &&
	 next_channel->first_recycling != NULL){
	next_channel->first_recycling->prev = last_recycling;
	last_recycling->next = next_channel->first_recycling;
      }else{
	last_recycling->next = NULL;
      }
    }else{
      gboolean link_next, link_prev;

      if(prev_channel != NULL &&
	 prev_channel->last_recycling != NULL){
	link_next = TRUE;
      }else{
	link_next = FALSE;
      }
    
      if(next_channel != NULL &&
	 next_channel->first_recycling != NULL){
	link_prev = TRUE;
      }else{
	link_prev = FALSE;
      }
    
      if(link_next){
	if(link_prev){
	  next_channel->first_recycling->prev = prev_channel->last_recycling;
	  prev_channel->last_recycling->next = next_channel->first_recycling;
	}else{
	  prev_channel->last_recycling->next = NULL;
	}
      }else if(link_prev){
	next_channel->first_recycling->prev = NULL;
      }
    }
  }
  
  /* destroy old recycling */
  if(destroy_old && old_first_recycling != NULL){
    nth_recycling = old_first_recycling;

    while(nth_recycling != old_last_recycling->next){
      next_recycling = nth_recycling->next;
      
      g_object_unref(G_OBJECT(nth_recycling));
      
      nth_recycling = next_recycling;
    }
  }

  /* find and set parent */
  if(first_recycling != NULL){
    AgsAudio *audio;
    AgsChannel *output;

    guint audio_channel, line;
    
    /* find parent */
    parent = NULL;
    
    if(AGS_IS_OUTPUT(channel)){
      current = channel->link;
    }else{
      current = channel;
    }
    
    while(current != NULL &&
	  parent == NULL){
      /* get audio */
      audio = (AgsAudio *) current->audio;

      audio_channel = current->audio_channel;
      line = current->line;

      /* get nth */
      output = audio->output;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(output,
				  audio_channel);
      }else{
	current = ags_channel_nth(output,
				  line);
      }

      /* check if parent found */
      if(current != NULL){
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  /* set parent */
	  parent = current->first_recycling;
	}

	current = current->link;
      }else{
	break;
      }
    }
    
    /* apply parent */
    if(update){
      nth_recycling = first_recycling;

      stop_recycling = last_recycling->next;
    }else{
      if(first_recycling == old_first_recycling){
	nth_recycling = old_last_recycling;

	stop_recycling = last_recycling->next;
      }else{
	nth_recycling = first_recycling;

	if(old_first_recycling != NULL){
	  stop_recycling = old_first_recycling->next;
	}else{
	  stop_recycling = NULL;
	}
      }
    }

    /* parent - do it so */
    while(nth_recycling != stop_recycling){
      nth_recycling->parent = parent;

      nth_recycling = nth_recycling->next;
    }
  }

  /* add recall id */
  recall_id = audio->recall_id;

  while(recall_id != NULL){
    if(AGS_RECALL_ID(recall_id->data)->recycling != NULL &&
       AGS_IS_OUTPUT(AGS_RECYCLING(AGS_RECALL_ID(recall_id->data)->recycling)->channel)){
      recall_id = recall_id->next;
	    
      continue;
    }
	    
    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling\0", first_recycling,
				     "recycling-context\0", AGS_RECALL_ID(recall_id->data)->recycling_context,
				     NULL);
	  
    ags_channel_add_recall_id(channel,
			      current_recall_id);
	    
    recall_id = recall_id->next;
  }

  /* update recycling container */
  recall_id = audio->recall_id;
    
  while(recall_id != NULL){
    is_output = AGS_IS_OUTPUT(channel);

    if(is_output){
      if((AgsRecycling *) AGS_RECALL_ID(recall_id->data)->recycling != old_first_recycling){
	recall_id = recall_id->next;
	
	continue;
      }
    }else{
      if((AgsRecycling *) AGS_RECALL_ID(recall_id->data)->recycling != old_first_recycling){
	recall_id = recall_id->next;
	
	continue;
      }
    }

    old_recycling_context = (AgsRecyclingContext *) AGS_RECALL_ID(recall_id->data)->recycling_context;
        
    recycling_context = ags_recycling_context_reset_recycling(old_recycling_context,
							      old_first_recycling, old_last_recycling,
							      first_recycling, last_recycling);
    
    if(recycling_context != NULL){
      recycling_context->parent = old_recycling_context->parent;

      ags_audio_add_recycling_context(audio,
				      recycling_context);
    }
    
    ags_audio_remove_recycling_context(audio,
				       (GObject *) old_recycling_context);

    g_object_set(recall_id->data,
		 "recycling-context\0", recycling_context,
		 NULL);
    
    ags_channel_recursive_reset_recycling_context(channel,
						  old_recycling_context,
						  recycling_context);
    
    recall_id = recall_id->next;
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

    flags = audio->flags;
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      output = audio->output;
    
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	guint audio_channel;
	
	audio_channel = channel->audio_channel;
	
	output = ags_channel_nth(output, audio_channel);
      }else{
	guint line;

	line = channel->line;
	
	output = ags_channel_nth(output, line);
      }
      
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
 * Resets the recycling container context. Asynchronously only.
 *
 * Since: 0.4
 */
void
ags_channel_recursive_reset_recycling_context(AgsChannel *channel,
					      AgsRecyclingContext *old_recycling_context,
					      AgsRecyclingContext *recycling_context)
{
  AgsAudio *audio;

  gboolean is_output;

  auto void ags_channel_reset_recycling_context(GList *recall_id);
  auto void ags_channel_recursive_reset_recycling_context_up(AgsChannel *channel);
  auto void ags_channel_recursive_reset_recycling_context_down_input(AgsChannel *channel);
  auto void ags_channel_recursive_reset_recycling_context_down_output(AgsChannel *channel);

  void ags_channel_reset_recycling_context(GList *recall_id){
    while(recall_id != NULL){
      if(AGS_RECALL_ID(recall_id->data)->recycling_context == old_recycling_context){
	g_object_set(recall_id->data,
		     "recycling-context\0", recycling_context,
		     NULL);
      }
      
      recall_id = recall_id->next;
    }
  }
  void ags_channel_recursive_reset_recycling_context_up(AgsChannel *channel){
    AgsAudio *audio;
    AgsChannel *current, *output;

    guint flags;
    guint audio_channel, line;
    gboolean is_output;
    
    if(channel == NULL){
      return;
    }

    current = channel;

    /* get audio and defaults */
    audio_channel = channel->audio_channel;
    line = channel->line;

    audio = (AgsAudio *) channel->audio;

    is_output = AGS_IS_OUTPUT(channel);

    if(is_output){
      /* reset recycling container */
      flags = audio->flags;
      
      ags_channel_reset_recycling_context(audio->recall_id);

      goto ags_channel_recursive_reset_recycling_context_up_OUTPUT;
    }

    /* reset current channel */
    while(current != NULL){
      audio = (AgsAudio *) current->audio;

      audio_channel = current->audio_channel;
      line = current->line;
      
      ags_channel_reset_recycling_context(current->recall_id);

      /* reset recycling container */
      flags = audio->flags;
      output = audio->output;
      
      ags_channel_reset_recycling_context(audio->recall_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	break;
      }

      current = ags_channel_nth(output,
				((AGS_AUDIO_ASYNC & (flags)) ? audio_channel: line));
      
    ags_channel_recursive_reset_recycling_context_up_OUTPUT:

      ags_channel_reset_recycling_context(current->recall_id);

      /* iterate */
      current = current->link;
    }

    /* update all in recycling context */
    if(AGS_IS_INPUT(current) &&
       (AGS_AUDIO_ASYNC & (flags)) != 0){
      AgsChannel *input, *link;

      input = audio->input;

      input = ags_channel_nth(input,
			      audio_channel);

      while(input != NULL){
	if(input != current){
	  link = input->link;
	  ags_channel_reset_recycling_context(input->recall_id);

	  /* traverse the tree */
	  ags_channel_recursive_reset_recycling_context_down_output(link);
	}

	/* iterate */
	input = input->next_pad;
      }
    }
  }
  void ags_channel_recursive_reset_recycling_context_down_input(AgsChannel *output){
    AgsAudio *audio;
    AgsChannel *input;
    AgsChannel *link;

    guint flags;
    guint audio_channel, line;

    pthread_mutex_t *mutex;
    pthread_mutex_t *audio_mutex, *input_mutex;
    
    if(output == NULL){
      return;
    }

    /* get audio and defaults */
    audio = (AgsAudio *) output->audio;

    audio_channel = output->audio_channel;
    line = output->line;

    /* reset recycling container */
    input = audio->input;

    flags = audio->flags;

    if((AGS_AUDIO_ASYNC & (flags)) != 0){
      input = ags_channel_nth(input,
			      audio_channel);

      while(input != NULL){
	/* reset recycling container */
	link = input->link;
	
	ags_channel_reset_recycling_context(input->recall_id);

	ags_channel_recursive_reset_recycling_context_down_output(link);

	/* iterate */
	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(input,
			      line);

      /* reset recycling container */
      link = input->link;
      
      ags_channel_reset_recycling_context(input->recall_id);

      ags_channel_recursive_reset_recycling_context_down_output(link);
    }
  }
  void ags_channel_recursive_reset_recycling_context_down_output(AgsChannel *output){
    AgsAudio *audio;

    guint flags;
    
    pthread_mutex_t *mutex;
    pthread_mutex_t *audio_mutex;

    if(output == NULL){
      return;
    }

    /* get audio and defaults */
    audio = (AgsAudio *) output->audio;

    /* check if still reset recycling container */
    flags = audio->flags;

    if(output != channel &&
       (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      return;
    }

    /* reset recycling container - output */
    ags_channel_reset_recycling_context(output->recall_id);

    /* reset recycling container - audio */
    ags_channel_reset_recycling_context(audio->recall_id);

    /* traverse the tree */
    ags_channel_recursive_reset_recycling_context_down_input(output);
  }

  /* entry point */
  if(channel == NULL){
    return;
  }

  /* traverse the tree */
  is_output = AGS_IS_OUTPUT(channel);
  
  if(!is_output){
    ags_channel_recursive_reset_recycling_context_down_output(channel->link);
    ags_channel_recursive_reset_recycling_context_up(channel);
  }else{
    ags_channel_recursive_reset_recycling_context_down_output(channel);
    ags_channel_recursive_reset_recycling_context_up(channel->link);
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
 * Since: 0.3
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
 * Since: 0.3
 */
AgsRecallID*
ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				gboolean arrange_recall_id, gboolean duplicate_templates,
				gboolean do_playback, gboolean do_sequencer, gboolean do_notation,
				gboolean resolve_dependencies,
				AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallID *audio_recall_id, *default_recall_id, *retval;
  gint stage_stop;

  auto AgsRecallID* ags_channel_recursive_play_init_arrange_recall_id_up(AgsChannel *channel,
									 AgsRecallID *recall_id,
									 gboolean initially_unowned);
  auto AgsRecallID** ags_channel_recursive_play_init_arrange_recall_id_down_input(AgsChannel *output,
										  AgsRecallID *recall_id,
										  gboolean initially_unowned);
  auto AgsRecallID* ags_channel_recursive_play_init_arrange_recall_id_down(AgsChannel *output,
									   AgsRecallID *recall_id,
									   gboolean initially_unowned);

  auto void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
							 AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
								 AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
							   AgsRecallID *recall_id);

  auto void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_resolve_down_input(AgsChannel *output,
							       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
							 AgsRecallID *recall_id);
  
  auto void ags_channel_recursive_play_init_up(AgsChannel *channel,
					       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_down(AgsChannel *output,
						 AgsRecallID *recall_id);
  
  /*
   * arrangeing recall ids is done from the axis to the root and then from the axis to the leafs
   */
  AgsRecallID* ags_channel_recursive_play_init_arrange_recall_id_up(AgsChannel *channel,
								    AgsRecallID *recall_id,
								    gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *audio_recall_id;
    AgsRecycling *recycling;
    AgsRecyclingContext *recycling_context;
    gint recycling_length;
    gint i;

    if(channel == NULL){
      return;
    }
    
    audio = AGS_AUDIO(channel->audio);

    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      /* create toplevel recycling container */
      recycling_length = ags_recycling_position(channel->first_recycling, channel->last_recycling->next,
						channel->last_recycling);
      recycling_length += 1;
	  
      recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "length\0", recycling_length,
							       "parent\0", NULL,
							       NULL);
      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);

      /* recycling */
      recycling = channel->first_recycling;

      for(i = 0; i < recycling_length; i++){
	recycling_context->recycling[i] = recycling;

	recycling = recycling->next;
      }

      /* output recall id */
      retval = 
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", channel->first_recycling,
				 "recycling-context\0", recycling_context,
				 NULL);
      g_object_set(recycling_context,
		   "recall_id\0", recall_id,
		   NULL);
      ags_channel_add_recall_id(channel,
				recall_id);

      if(do_playback){
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
      
      /* audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context\0", recall_id->recycling_context,
				     "recycling\0", channel->first_recycling,
				     NULL);
      ags_audio_add_recall_id(audio,
			      audio_recall_id);
      
      if(do_playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      goto ags_channel_recursive_play_init_up_OUTPUT;
    }else{
      /* recycling container */
      recycling_length = ags_recycling_position(channel->first_recycling, channel->last_recycling->next,
						channel->last_recycling);
      recycling_length += 1;
	  
      recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "length\0", recycling_length,
							       "parent\0", NULL,
							       NULL);
      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);

      /* recycling */
      recycling = channel->first_recycling;

      for(i = 0; i < recycling_length; i++){
	recycling_context->recycling[i] = recycling;

	recycling = recycling->next;
      }

      /* input recall id */
      retval = 
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", channel->first_recycling,
				 "recycling-context\0", recycling_context,
				 NULL);
      g_object_set(recycling_context,
		   "recall_id\0", recall_id,
		   NULL);
      ags_audio_add_recall_id(audio,
			      (GObject *) recall_id);

      if(do_playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
      
      /* audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context\0", recall_id->recycling_context,
				     "recycling\0", channel->first_recycling,
				     NULL);
      ags_audio_add_recall_id(audio,
			      (GObject *) audio_recall_id);
      
      if(do_playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* input recall id */
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling\0", current->first_recycling,
			       "recycling-context\0", recall_id->recycling_context,
			       NULL);
      g_object_set(recycling_context,
		   "recall_id\0", recall_id,
		   NULL);
      ags_channel_add_recall_id(current,
				(GObject *) recall_id);

      if(do_playback){
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* audio recall id */
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling\0", current->first_recycling,
			       "recycling-context\0", recall_id->recycling_context,
			       NULL);
      ags_audio_add_recall_id(audio,
			      (GObject *) recall_id);

      if(do_playback){
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_recursive_play_init_up_OUTPUT:
      if(current != channel){
	/* output recall id with default recycling container */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", current->first_recycling,
				 "recycling-context\0", recall_id->recycling_context,
				 NULL);
	ags_channel_add_recall_id(current,
				  (GObject *) recall_id);

	if(do_playback){
	  recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if(do_sequencer){
	  recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if(do_notation){
	  recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}
      }
      
      /* iterate */
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }

    return(audio_recall_id);
  }
  AgsRecallID** ags_channel_recursive_play_init_arrange_recall_id_down_input(AgsChannel *output,
									     AgsRecallID *default_recall_id,
									     gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *input_recall_id;
    AgsRecallID **recall_id;
    AgsRecyclingContext *recycling_context;

    if(output == NULL){
      return(NULL);
    }
    
    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return(NULL);
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);

      recall_id = (AgsRecallID **) malloc((audio->input_pads + 1) * sizeof(AgsRecallID*));
      recall_id[audio->input_pads] = NULL;
    
      while(input != NULL){
	/* input recall id */
	input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context\0", default_recall_id->recycling_context,
				       "recycling\0", input->first_recycling,
				       NULL);
	recall_id[input->pad] = input_recall_id;
	ags_channel_add_recall_id(input,
				  (GObject *) input_recall_id);

	if(do_playback){
	  input_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if(do_sequencer){
	  input_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if(do_notation){
	  input_recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}

	/* follow the links */
	if(input->link != NULL){
	  ags_channel_recursive_play_init_arrange_recall_id_down(input->link,
								 input_recall_id,
								 FALSE);
	}

	/* iterate */
	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      recall_id = (AgsRecallID **) malloc(2 * sizeof(AgsRecallID*));
      recall_id[1] = NULL;

      /* input recall id */
      recall_id[0] = 
	input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context\0", default_recall_id->recycling_context,
				       "recycling\0", input->first_recycling,
				       NULL);
      ags_channel_add_recall_id(input,
				(GObject *) input_recall_id);

      if(do_playback){
	input_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	input_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	input_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }


      /* follow the links */
      if(input->link != NULL){
	ags_channel_recursive_play_init_arrange_recall_id_down(input->link,
							       input_recall_id,
							       FALSE);
      }
    }

    return(recall_id);
  }
  AgsRecallID* ags_channel_recursive_play_init_arrange_recall_id_down(AgsChannel *output,
								      AgsRecallID *recall_id,
								      gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsChannel *input, *last_input;
    AgsRecycling *recycling;
    AgsRecyclingContext *recycling_context;
    AgsRecallID *output_recall_id, *audio_recall_id, *default_recall_id;
    AgsRecallID **input_recall_id;
    gint recycling_length;
    gint i;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);
    recycling_context = recall_id->recycling_context;

    /* input */
    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
      last_input = ags_channel_pad_last(input);
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
      last_input = input;
    }

    /* AgsOutput */
    /* recall id */
    if(!initially_unowned){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	/* output recycling container */
	recycling_length = ags_recycling_position(output->first_recycling, output->last_recycling->next,
						  output->last_recycling);
	recycling_length += 1;

	recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
								 "parent\0", recycling_context,
								 "length\0", recycling_length,
								 NULL);
	ags_audio_add_recycling_context(audio,
					(GObject *) recycling_context);

	/* set up recycling container */
	recycling = output->first_recycling;
	
	for(i = 0; i < recycling_length; i++){
	  recycling_context->recycling[i] = recycling;
	
	  recycling = recycling->next;
	}
      }

      /* output recall id */
      output_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				      "recycling-context\0", recycling_context,
				      "recycling\0", output->first_recycling,
				      NULL);
      ags_channel_add_recall_id(output,
				(GObject *) output_recall_id);

      if(do_playback){
	output_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	output_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	output_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	g_object_set(recycling_context,
		     "recall_id\0", output_recall_id,
		     NULL);
      }

      /* create audio recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context\0", recycling_context,
				     "recycling\0", output->first_recycling,
				     NULL);
      ags_audio_add_recall_id(audio,
			      (GObject *) audio_recall_id);

      if(do_playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
    }else{
      /* get audio recall id */
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recycling_context);
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* input recycling container */
      recycling_length = ags_recycling_position(input->first_recycling, last_input->last_recycling->next,
						last_input->last_recycling);
      recycling_length += 1;

      recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "parent\0", recycling_context,
							       "length\0", recycling_length,
							       NULL);
      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);

      /* set up recycling container */
      recycling = input->first_recycling;
      
      for(i = 0; i < recycling_length; i++){
	recycling_context->recycling[i] = recycling;

	recycling = recycling->next;
      }
    }

    /* audio recall id */
    default_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context\0", recycling_context,
				     "recycling\0", input->first_recycling,
				     NULL);
    ags_audio_add_recall_id(audio,
			    (GObject *) default_recall_id);

    if(do_playback){
      default_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
    }

    if(do_sequencer){
      default_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
    }

    if(do_notation){
      default_recall_id->flags |= AGS_RECALL_ID_NOTATION;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_object_set(recycling_context,
		   "recall_id\0", default_recall_id,
		   NULL);
    }

    /* follow the links */
    input_recall_id = ags_channel_recursive_play_init_arrange_recall_id_down_input(output,
										   default_recall_id,
										   FALSE);
    if(input_recall_id != NULL){
      free(input_recall_id);
    }

    return(default_recall_id);
  }

  /*
   * duplicateing template recalls is done from the root to the leafes
   *
   * externalized 
   * externalized 
   */

  void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
						    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL){
      return;
    }
    
    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_duplicate_up_OUTPUT;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      ags_channel_duplicate_recall(current,
				   recall_id);

      /* AgsAudio */
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);

      ags_audio_duplicate_recall(audio,
				 recall_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

    ags_channel_recursive_play_init_duplicate_up_OUTPUT:
      ags_channel_duplicate_recall(current,
				   recall_id);

      if(current == channel){
	/* AgsAudio */
	recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							 recall_id->recycling_context);

	ags_audio_duplicate_recall(audio,
				   recall_id);
      }
      
      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
							    AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsRecallID *input_recall_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input,
				output->audio_channel);
          
      /* duplicate recalls on input */
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       default_recall_id->recycling_context);

	/* duplicate input */
	ags_channel_duplicate_recall(input,
				     input_recall_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 default_recall_id->recycling_context);

	  /* follow the links */
	  ags_channel_recursive_play_init_duplicate_down(input->link,
							 input_recall_id);
	}

	/* iterate */
	input = input->next_pad;
      }

    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      
      /* duplicate recalls on input */
      ags_channel_duplicate_recall(input,
				   input_recall_id);

      /* follow the links */
      if(input->link != NULL){
	/* follow the links */
	ags_channel_recursive_play_init_duplicate_down(input->link,
						       input_recall_id);
      }
    }
  }
  void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
						      AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id, *default_recall_id;
    GList *list;  

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if(recall_id->recycling_context->parent != NULL){
      list = output->recall_id;
	
      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	  recall_id = list->data;
	  break;
	}
	  
	list = list->next;
      }

      if(list == NULL){
	recall_id = NULL;
      }
    }

    /* duplicate output */
    ags_channel_duplicate_recall(output,
				 recall_id);

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsChannel *input;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
      }

      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }
      
      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 audio_recall_id);

      ags_audio_duplicate_recall(audio,
				 default_recall_id);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   default_recall_id);
    }else{
      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 audio_recall_id);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   default_recall_id);
    }
  }

  /*
   * resolving recall dependencies has to be done from the root to the leafs
   */
  void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						  AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    
    if(channel == NULL){
      return;
    }

    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_resolve_up0;
    }

    while(current != NULL){
      /* resolve input */
      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      ags_channel_resolve_recall(current,
				 recall_id);
      
      /* resolve audio */
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);

      ags_audio_resolve_recall(audio,
			       recall_id);

      /* retrieve output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

      /* stop if output has recycling */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	return;
      }

      /* resolve output */
      recall_id = ags_recall_id_find_recycling_context(channel->recall_id,
						       recall_id->recycling_context);

    ags_channel_recursive_play_init_resolve_up0:
      ags_channel_resolve_recall(current,
				 recall_id);
      
      if(current == channel){
	AgsRecallID *audio_recall_id;

	/* AgsAudio */
	audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       recall_id->recycling_context);

	ags_audio_resolve_recall(audio,
				 audio_recall_id);
      }

      /* iterate */      
      if(current->link == NULL)
	break;
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_resolve_down_input(AgsChannel *output,
							  AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsRecallID *input_recall_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input, output->audio_channel);
      
      /* resolve recall dependencies on input */
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       default_recall_id->recycling_context);

	/* resolve input */
	ags_channel_resolve_recall(input,
				   input_recall_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 default_recall_id->recycling_context);

	  /* traverse the tree */
	  if(input->link != NULL){
	    /* follow the links */
	    ags_channel_recursive_play_init_resolve_down(input->link,
							 input_recall_id);
	  }
	}

	/* iterate */
	input = input->next_pad;
      }
    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							     default_recall_id->recycling_context);

      /* resolve recall dependencies on input */
      ags_channel_resolve_recall(input,
				 input_recall_id);

      /* traverse the tree */
      if(input->link != NULL){

	/* follow the links */
	ags_channel_recursive_play_init_resolve_down(input->link,
						     input_recall_id);
      }
    }
  }
  void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
						    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *audio_recall_id, *default_recall_id;
    GList *list;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if(recall_id->recycling_context->parent != NULL){
      list = output->recall_id;

      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	  recall_id = list->data;
	  break;
	}

	list = list->next;
      }

      if(list == NULL){
	recall_id = NULL;
      }
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
    }

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }

      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;
    }

    /* resolve dependencies on output */
    ags_channel_resolve_recall(output,
			       recall_id);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       audio_recall_id);
      
      ags_audio_resolve_recall(audio,
			       default_recall_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 default_recall_id);
    }else{
      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       audio_recall_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 default_recall_id);
    }
  }

  /*
   * run-initializing recalls is done from the leafs to the root
   */
  void ags_channel_recursive_play_init_up(AgsChannel *channel,
					  AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL){
      return;
    }

    audio = AGS_AUDIO(channel->audio);

    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_up0;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;

      /* AgsInput */
      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      ags_channel_init_recall(current, stage,
			      recall_id);
      
      /* AgsAudio */
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);

      ags_audio_init_recall(audio, stage,
			    recall_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

    ags_channel_recursive_play_init_up0:
      ags_channel_init_recall(current, stage,
			      recall_id);

      if(current == channel){
	AgsRecallID *audio_recall_id;

	/* AgsAudio */
	audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       recall_id->recycling_context);

	ags_audio_init_recall(audio, stage,
			      audio_recall_id);
      }


      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
            
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						  AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *input_recall_id;
    
    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
    
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       default_recall_id->recycling_context);

	/* follow the links */
	if(input->link != NULL){
	  ags_channel_recursive_play_init_down(input->link,
					       input_recall_id);
	}
	
	/* init recall */
	ags_channel_init_recall(input, stage,
				input_recall_id);

	/* iterate */
	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
      
      input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							     default_recall_id->recycling_context);
      
      /* follow the links */
      if(input->link != NULL){
	ags_channel_recursive_play_init_down(input->link,
					     input_recall_id);
      }

      /* init recall */
      ags_channel_init_recall(input, stage,
			      input_recall_id);
    }
  }
  void ags_channel_recursive_play_init_down(AgsChannel *output,
					    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *default_recall_id, *audio_recall_id;
    GList *list;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if(recall_id->recycling_context->parent != NULL){
      list = output->recall_id;

      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	  recall_id = list->data;
	  break;
	}

	list = list->next;
      }

      if(list == NULL){
	recall_id = NULL;
      }
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
    }

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }

      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;
    }


    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_audio_init_recall(audio, stage,
			    audio_recall_id);

      ags_audio_init_recall(audio, stage,
			    default_recall_id);

      /* follow the links */
      ags_channel_recursive_play_init_down_input(output,
						 default_recall_id);
    }else{
      ags_audio_init_recall(audio, stage,
			    audio_recall_id);

      /* follow the links */
      ags_channel_recursive_play_init_down_input(output,
						 default_recall_id);
    }

    /* init recall*/
    ags_channel_init_recall(output, stage,
			    recall_id);
  }


  /*
   * entry point
   */
  if(channel == NULL){
    return(NULL);
  }
  
  audio = AGS_AUDIO(channel->audio);

  /* arrange recall id */
  if(arrange_recall_id){
    if(AGS_IS_OUTPUT(channel)){
      /* follow the links */
      recall_id = ags_channel_recursive_play_init_arrange_recall_id_up(channel,
								       NULL,
								       FALSE);
    }else{
      AgsChannel *input;
      AgsRecycling *recycling;
      AgsRecyclingContext *recycling_context;
      guint recycling_length;
      guint i;

      input = channel;

      /* input recycling container */
      recycling_length = ags_recycling_position(input->first_recycling, input->last_recycling->next,
						input->last_recycling);
      recycling_length += 1;

      recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "parent\0", NULL,
							       "length\0", recycling_length,
							       NULL);
      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);

      /* set up recycling container */
      recycling = input->first_recycling;
      
      for(i = 0; i < recycling_length; i++){
	recycling_context->recycling[i] = recycling;

	recycling = recycling->next;
      }

      /* audio recall id */
      default_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context\0", recycling_context,
				       "recycling\0", input->first_recycling,
				       NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   (GObject *) default_recall_id);

      if(do_playback){
	default_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	default_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	default_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	g_object_set(recycling_context,
		     "recall_id\0", default_recall_id,
		     NULL);
      }

      /* input recall id */
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling-context\0", default_recall_id->recycling_context,
			       "recycling\0", input->first_recycling,
			       NULL);
      input->recall_id = ags_recall_id_add(input->recall_id,
					   (GObject *) recall_id);

      if(do_playback){
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(do_sequencer){
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(do_notation){
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
    }
  }

  /* get audio recall id */
  audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							 recall_id->recycling_context);

  /* arrange recall id */
  if(arrange_recall_id){
    AgsRecallID *next_recall_id;

    if(AGS_IS_OUTPUT(channel)){
      next_recall_id = ags_channel_recursive_play_init_arrange_recall_id_down(channel,
									      audio_recall_id,
									      TRUE);
    }else{
      AgsChannel *input;

      input = channel;
      next_recall_id = ags_channel_recursive_play_init_arrange_recall_id_down(input->link,
									      recall_id,
									      FALSE);
    }
  }

  /* get default recall id */
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
    default_recall_id = audio_recall_id;
  }else{
    if(AGS_IS_OUTPUT(channel)){
      AgsChannel *input;
      GList *list;

      if((AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	input = ags_channel_nth(audio->input,
				channel->line);
      }else{
	input = ags_channel_nth(audio->input,
				channel->audio_channel);
      }

      list = recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));
    }
  }

  /* duplicate AgsRecall templates */
  if(duplicate_templates){
    if(AGS_IS_OUTPUT(channel)){
      ags_channel_recursive_play_init_duplicate_up(channel->link,
						   recall_id);

      ags_channel_recursive_play_init_duplicate_down(channel,
						     recall_id);
    }else{
      /* duplicate input */
      ags_channel_duplicate_recall(channel,
				   recall_id);

      /* follow the links */
      if(channel->link != NULL){
	ags_channel_recursive_play_init_duplicate_down(channel->link,
						       recall_id);
      }
    }
  }

  /* resolve dependencies */
  if(resolve_dependencies){
    if(AGS_IS_OUTPUT(channel)){
      ags_channel_recursive_play_init_resolve_up(channel->link,
						 recall_id);

      ags_channel_recursive_play_init_resolve_down(channel,
						   recall_id);
    }else{
      /* resolve input */
      ags_channel_resolve_recall(channel,
				 recall_id);

      /* follow the links */
      if(channel->link != NULL){
	ags_channel_recursive_play_init_resolve_down(channel->link,
						     default_recall_id);
      }
    }
  }

  /* do the different stages of recall initializiation */
  if(stage == -1){
    stage = 0;
    stage_stop = 3;
  }else{
    stage_stop = stage + 1;
  }

  if(stage == 0){
    if(do_playback){
      AGS_PLAYBACK(channel->playback)->recall_id[0] = recall_id;
      g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
		      AGS_PLAYBACK_PLAYBACK);
    }

    if(do_sequencer){
      AGS_PLAYBACK(channel->playback)->recall_id[1] = recall_id;
      g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
		      AGS_PLAYBACK_SEQUENCER);
    }

    if(do_notation){
      AGS_PLAYBACK(channel->playback)->recall_id[2] = recall_id;
      g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
		      AGS_PLAYBACK_NOTATION);
    }
  }

  for(; stage < stage_stop;){
    if(AGS_IS_OUTPUT(channel)){
      ags_channel_recursive_play_init_down(channel,
					   recall_id);

      ags_channel_recursive_play_init_up(channel->link,
					 recall_id);
    }else{
      if(channel->link != NULL){
	ags_channel_recursive_play_init_down(channel->link,
					     recall_id);
      }

      /* init recall */
      ags_channel_init_recall(channel, stage,
			      recall_id);
    }
    
    stage++;
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
 * Since: 0.4
 */
void
ags_channel_recursive_play_threaded(AgsChannel *channel,
				    AgsRecallID *recall_id, gint stage)
{
  AgsChannel *link;
  
  AgsMutexManager *mutex_manager;
  AgsRecyclingThread *recycling_thread;
  
  guint playback_mode;
  gboolean is_output;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  auto void ags_channel_recursive_play_threaded_up(AgsChannel *channel,
						   AgsRecallID *recall_id,
						   AgsRecyclingThread *recycling_thread);
  auto void ags_channel_recursive_play_threaded_down_input(AgsChannel *output,
							   AgsRecallID *default_recall_id,
							   AgsRecyclingThread *recycling_thread);
  auto void ags_channel_recursive_play_threaded_down(AgsChannel *output,
						     AgsRecallID *recall_id,
						     AgsRecyclingThread *recycling_thread);

  void ags_channel_recursive_play_threaded_up(AgsChannel *channel,
					      AgsRecallID *recall_id,
					      AgsRecyclingThread *recycling_thread)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecycling *first_recycling;
    
    guint flags;
    guint line, audio_channel;
    gboolean is_output;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *current_mutex;
    
    if(channel == NULL ||
       recall_id == NULL){
      return;
    }

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    current_mutex = 
      mutex = ags_mutex_manager_lookup(mutex_manager,
				       channel);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio and check if first is output */
    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) channel->audio;
    first_recycling = channel->first_recycling;
    
    is_output = AGS_IS_OUTPUT(channel);
    
    pthread_mutex_unlock(mutex);

    /* start iteration */
    current = channel;

    /* lock context */
    //    ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
    
    if(is_output){
      goto ags_channel_recursive_play_threaded_up_OUTPUT;
    }
    
    /* go to toplevel AgsChannel */
    while(current != NULL){
      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       current);
    
      pthread_mutex_unlock(application_mutex);

      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;

      /* play input */
      pthread_mutex_lock(current_mutex);

      audio = AGS_AUDIO(current->audio);
      
      audio_channel = current->audio_channel;
      line = current->audio_channel;

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      pthread_mutex_unlock(current_mutex);

      ags_recycling_thread_play_channel(recycling_thread,
					(GObject *) current,
					recall_id,
					stage);

      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     audio);
    
      pthread_mutex_unlock(application_mutex);

      /* play audio */
      pthread_mutex_lock(audio_mutex);
      
      flags = audio->flags;

      current = audio->output;

      pthread_mutex_unlock(audio_mutex);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	return;
      }
      
      pthread_mutex_lock(audio_mutex);

      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);

      pthread_mutex_unlock(audio_mutex);

      /* get output */
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	current = ags_channel_nth(current,
				  audio_channel);
      }else{
	current = ags_channel_nth(current,
				  line);
      }
      
      ags_recycling_thread_play_audio(recycling_thread,
				      (GObject *) current, (GObject *) audio,
				      recall_id,
				      stage);

      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       current);
    
      pthread_mutex_unlock(application_mutex);

      /* play output */
      pthread_mutex_lock(current_mutex);

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      pthread_mutex_unlock(current_mutex);
      
    ags_channel_recursive_play_threaded_up_OUTPUT:
      ags_recycling_thread_play_channel(recycling_thread,
					(GObject *) current,
					recall_id,
					stage);

      /* iterate */
      pthread_mutex_lock(current_mutex);
      
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }

    /* unlock context */
    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
  }

  void ags_channel_recursive_play_threaded_down_input(AgsChannel *output,
						      AgsRecallID *default_recall_id,
						      AgsRecyclingThread *recycling_thread)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsChannel *link;
    AgsRecycling *first_recycling;
    AgsRecyclingContext *recycling_context;
    AgsRecallID *input_recall_id;
    
    AgsRecyclingThread *input_recycling_thread;

    guint flags;
    guint audio_channel, line;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *input_mutex;

    if(output == NULL || default_recall_id == NULL){
      return;
    }
    
    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     output);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio */
    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) output->audio;

    audio_channel = output->audio_channel;
    line = output->line;

    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   audio);
    
    pthread_mutex_unlock(application_mutex);

    /* retrieve input and flags */
    pthread_mutex_lock(audio_mutex);
    
    input = audio->input;
    recycling_context = default_recall_id->recycling_context;
    
    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);
    
    if(input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(input,
				audio_channel);
      
      /* play recalls on input */
      while(input != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);

	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       input);
    
	pthread_mutex_unlock(application_mutex);

	/* get first recycling */
	pthread_mutex_lock(input_mutex);
	
	first_recycling = input->first_recycling;

	pthread_mutex_unlock(input_mutex);
	
	/* lock context */
	//	ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
	
	/* find recycling thread */
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	  input_recycling_thread = ags_recycling_thread_find_child(recycling_thread,
								   first_recycling);
	}else{
	  input_recycling_thread = recycling_thread;
	}
	
	/* find input recall id */
	pthread_mutex_lock(input_mutex);
	
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       recycling_context);

	pthread_mutex_unlock(input_mutex);
	
	/* play input */
	ags_recycling_thread_play_channel(input_recycling_thread,
					  (GObject *) input,
					  input_recall_id,
					  stage);

	/* unlock context */
	//	ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));

	/* iterate */
	pthread_mutex_lock(input_mutex);
	
	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);

	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       input);
    
	pthread_mutex_unlock(application_mutex);

	/* get link */
	pthread_mutex_lock(input_mutex);
	
	link = input->link;
	
	pthread_mutex_unlock(input_mutex);
	
	if(link != NULL){
	  /* get first recycling */
	  pthread_mutex_lock(input_mutex);
	
	  first_recycling = input->first_recycling;
	  
	  pthread_mutex_unlock(input_mutex);

	  /* find recycling thread */
	  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	    input_recycling_thread = ags_recycling_thread_find_child(recycling_thread,
								     first_recycling);
	  }else{
	    input_recycling_thread = recycling_thread;
	  }
	  
	  /* find input recall id */
	  pthread_mutex_lock(input_mutex);
	  
	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 recycling_context);

	  pthread_mutex_unlock(input_mutex);

	  /* traverse the tree */
	  if(link != NULL){
	    ags_channel_recursive_play_threaded_down(link,
						     input_recall_id,
						     input_recycling_thread);
	  }
	}

	/* iterate */
	pthread_mutex_lock(input_mutex);
	
	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }
    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(input,
			      line);

      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      input_mutex = ags_mutex_manager_lookup(mutex_manager,
					     input);
    
      pthread_mutex_unlock(application_mutex);

      /* get first recycling, find input recall id and get link */
      pthread_mutex_lock(input_mutex);

      first_recycling = input->first_recycling;
      input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							     recycling_context);

      link = input->link;

      pthread_mutex_unlock(input_mutex);

      /* traverse the tree */
      if(link != NULL){
	ags_channel_recursive_play_threaded_down(link,
						 input_recall_id,
						 input_recycling_thread);
      }

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));

      /* find recycling thread */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	input_recycling_thread = ags_recycling_thread_find_child(recycling_thread,
								 first_recycling);
      }else{
	input_recycling_thread = recycling_thread;
      }
      
      /* play recalls on input */
      ags_recycling_thread_play_channel(input_recycling_thread,
					(GObject *) input,
					input_recall_id,
					stage);

      //      ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
    }
  }
  
  void ags_channel_recursive_play_threaded_down(AgsChannel *output,
						AgsRecallID *recall_id,
						AgsRecyclingThread *recycling_thread)
  {
    AgsAudio *audio;
    AgsRecycling *first_recycling;
    AgsRecallID *default_recall_id, *audio_recall_id;

    GList *list;

    guint flags;
    guint line, audio_channel;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *current_mutex;
    
    if(output == NULL ||
       recall_id == NULL){
      return;
    }

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     channel);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio */
    pthread_mutex_lock(mutex);

    audio = (AgsAudio *) output->audio;
    first_recycling = output->first_recycling;
    
    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   audio);
    
    pthread_mutex_unlock(application_mutex);

    /* lock context */
    //    ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));

    /* retrieve recall id */
    pthread_mutex_lock(mutex);

    flags = audio->flags; 

    if(recall_id->recycling_context->parent != NULL){
      list = output->recall_id;

      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	  recall_id = list->data;
	  break;
	}

	list = list->next;
      }

      if(list == NULL){
	recall_id = NULL;
      }
    }

    pthread_mutex_unlock(mutex);
    
    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      pthread_mutex_lock(audio_mutex);
      
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }

      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));

      pthread_mutex_unlock(audio_mutex);
    }else{
      pthread_mutex_lock(audio_mutex);

      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;

      pthread_mutex_unlock(audio_mutex);
    }
    
    /* unlock context */
    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      /* call function which play input */
      ags_channel_recursive_play_threaded_down_input(output,
						     default_recall_id,
						     recycling_thread);

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
    
      /* play audio */
      ags_recycling_thread_play_audio(recycling_thread,
				      (GObject *) output, (GObject *) audio,
				      audio_recall_id,
				      stage);

      ags_recycling_thread_play_audio(recycling_thread,
				      (GObject *) output, (GObject *) audio,
				      default_recall_id,
				      stage);
    }else{
      /* call function which play input */
      ags_channel_recursive_play_threaded_down_input(output,
						     default_recall_id,
						     recycling_thread);

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
      
      /* play audio */
      ags_recycling_thread_play_audio(recycling_thread,
				      (GObject *) output, (GObject *) audio,
				      audio_recall_id,
				      stage);
    }

    /* play output */
    ags_recycling_thread_play_channel(recycling_thread,
				      (GObject *) output,
				      recall_id,
				      stage);

    /* unlock context */
    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
  }

  /* entry point */
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

  /* recursive compute audio */
  pthread_mutex_lock(mutex);

  link = channel->link;

  /* retrieve playback mode and recycling thread */
  playback_mode = 0;
  
  if((AGS_RECALL_ID_PLAYBACK & (recall_id->flags)) != 0){
    playback_mode = 0;
  }else if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    playback_mode = 1;
  }else if((AGS_RECALL_ID_NOTATION & (recall_id->flags)) != 0){
    playback_mode = 2;
  }

  recycling_thread = AGS_RECYCLING_THREAD(AGS_PLAYBACK(channel->playback)->recycling_thread[playback_mode]);
  
  /* check if start is output */
  is_output = AGS_IS_OUTPUT(channel);

  pthread_mutex_unlock(mutex);

  /* traverse the tree */
  if(is_output){
    ags_channel_recursive_play_threaded_down(channel,
					     recall_id,
					     recycling_thread);

    ags_channel_recursive_play_threaded_up(link,
					   recall_id,
					   recycling_thread);
  }else{
    AgsAudio *audio;
    AgsRecallID *audio_recall_id, *default_recall_id;

    guint flags;
    
    pthread_mutex_t *audio_mutex;

    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) channel->audio;

    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
    
    pthread_mutex_unlock(application_mutex);

    /* get audio recall id */
    pthread_mutex_lock(audio_mutex);

    flags = audio->flags;
    
    audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							   recall_id->recycling_context);

    pthread_mutex_unlock(audio_mutex);

    /* get default recall id and default recycling thread*/
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      default_recall_id = audio_recall_id;
    }else{
      default_recall_id = recall_id;
    }

    /* follow the links */
    if(link != NULL){
      ags_channel_recursive_play_threaded_down(link,
					       default_recall_id,
					       recycling_thread);
    }

    /*  */
    ags_channel_recursive_play_threaded_up(channel,
					   recall_id,
					   recycling_thread);
  }
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
 * Since: 0.3
 */
void
ags_channel_recursive_play(AgsChannel *channel,
			   AgsRecallID *recall_id, gint stage)
{
  AgsChannel *link;

  AgsMutexManager *mutex_manager;

  gboolean is_output;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  auto void ags_channel_recursive_play_up(AgsChannel *channel,
					  AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_down_input(AgsChannel *output,
						  AgsRecallID *default_recall_id);
  auto void ags_channel_recursive_play_down(AgsChannel *output,
					    AgsRecallID *recall_id);

  void ags_channel_recursive_play_up(AgsChannel *channel,
				     AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecycling *first_recycling;

    guint flags;
    guint line, audio_channel;
    gboolean is_output;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *current_mutex;
    
    if(channel == NULL ||
       recall_id == NULL){
      return;
    }

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     channel);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio and check if first is output */
    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) channel->audio;
    first_recycling = channel->first_recycling;
    
    is_output = AGS_IS_OUTPUT(channel);
    
    pthread_mutex_unlock(mutex);

    /* start iteration */
    current = channel;

    if(is_output){
      pthread_mutex_lock(current_mutex);
      
      goto ags_channel_recursive_play_up_OUTPUT;
    }
    
    /* lock context */
    //    ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));

    /* go to toplevel AgsChannel */
    while(current != NULL){
      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       current);
    
      pthread_mutex_unlock(application_mutex);

      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;

      /* play input */
      pthread_mutex_lock(current_mutex);

      audio = AGS_AUDIO(current->audio);
      
      audio_channel = current->audio_channel;
      line = current->audio_channel;

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      pthread_mutex_unlock(current_mutex);

      ags_channel_play(current,
		       recall_id,
		       stage);
      
      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     audio);
    
      pthread_mutex_unlock(application_mutex);

      /* play audio */
      pthread_mutex_lock(audio_mutex);
      
      flags = audio->flags;

      current = audio->output;
      
      pthread_mutex_unlock(audio_mutex);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
	break;
      }
      
      pthread_mutex_lock(audio_mutex);

      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);
      
      pthread_mutex_unlock(audio_mutex);

      /* get output */
      if((AGS_AUDIO_ASYNC & (flags)) != 0){
	current = ags_channel_nth(current,
				  audio_channel);
      }else{
	current = ags_channel_nth(current,
				  line);
      }

      ags_audio_play(audio,
		     recall_id,
		     stage);
      
      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       current);
    
      pthread_mutex_unlock(application_mutex);

      /* play output */
      pthread_mutex_lock(current_mutex);

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);
      
    ags_channel_recursive_play_up_OUTPUT:
      ags_channel_play(current,
		       recall_id,
		       stage);

      pthread_mutex_unlock(current_mutex);
      
      /* iterate */
      pthread_mutex_lock(current_mutex);
      
      current = current->link;

      pthread_mutex_unlock(current_mutex);
    }

    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
  }
  
  void ags_channel_recursive_play_down_input(AgsChannel *output,
					     AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsChannel *link;
    AgsRecycling *first_recycling;
    AgsRecyclingContext *recycling_context;
    AgsRecallID *input_recall_id;

    guint flags;
    guint audio_channel, line;

    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *input_mutex;
    
    if(output == NULL || default_recall_id == NULL){
      return;
    }
    
    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     output);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio */
    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) output->audio;

    audio_channel = output->audio_channel;
    line = output->line;

    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   audio);
    
    pthread_mutex_unlock(application_mutex);

    /* retrieve input and flags */
    pthread_mutex_lock(audio_mutex);
    
    input = audio->input;
    recycling_context = default_recall_id->recycling_context;
    
    flags = audio->flags;

    pthread_mutex_unlock(audio_mutex);
    
    if(input == NULL){
      return;
    }
    
    if((AGS_AUDIO_ASYNC & (flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(input,
				audio_channel);

      /* play recalls on input */
      while(input != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);

	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       input);
    
	pthread_mutex_unlock(application_mutex);

	/* get first recycling */
	pthread_mutex_lock(input_mutex);
	
	first_recycling = input->first_recycling;

	pthread_mutex_unlock(input_mutex);
	
	/* lock context */
	//	ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
	
	/* find input recall id */
	pthread_mutex_lock(input_mutex);
	
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       recycling_context);

	pthread_mutex_unlock(input_mutex);

	/* play input */
	ags_channel_play(input,
			 input_recall_id,
			 stage);

	/* unlock context */
	//	ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));

	/* iterate */
	pthread_mutex_lock(input_mutex);
	
	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	/* lookup mutex */
	pthread_mutex_lock(application_mutex);

	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       input);
    
	pthread_mutex_unlock(application_mutex);

	/* get link */
	pthread_mutex_lock(input_mutex);
	
	link = input->link;
	
	pthread_mutex_unlock(input_mutex);
	
	if(link != NULL){
	  /* get first recycling */
	  pthread_mutex_lock(input_mutex);
	
	  first_recycling = input->first_recycling;
	  
	  pthread_mutex_unlock(input_mutex);

	  /* find input recall id */
	  pthread_mutex_lock(input_mutex);
	  
	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 default_recall_id->recycling_context);

	  pthread_mutex_unlock(input_mutex);

	  /* traverse the tree */
	  if(link != NULL){
	    ags_channel_recursive_play_down(input->link,
					    input_recall_id);
	  }
	}

	/* iterate */
	pthread_mutex_lock(input_mutex);
	
	input = input->next_pad;

	pthread_mutex_unlock(input_mutex);
      }
    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(input,
			      line);
      
      /* lookup mutex */
      pthread_mutex_lock(application_mutex);

      input_mutex = ags_mutex_manager_lookup(mutex_manager,
					     input);
    
      pthread_mutex_unlock(application_mutex);

      /* get first recycling, find input recall id and get link */
      pthread_mutex_lock(input_mutex);

      first_recycling = input->first_recycling;
      input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							     default_recall_id->recycling_context);

      link = input->link;

      pthread_mutex_unlock(input_mutex);

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
      
      /* play recalls on input */
      ags_channel_play(input,
		       input_recall_id,
		       stage);

      /* unlock context */
      //      ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
      
      /* traverse the tree */
      if(link != NULL){
	ags_channel_recursive_play_down(link,
					input_recall_id);
      }
    }
  }

  void ags_channel_recursive_play_down(AgsChannel *output,
				       AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecycling *first_recycling;
    AgsRecallID *default_recall_id, *audio_recall_id;

    GList *list;

    guint flags;
    guint line, audio_channel;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *mutex, *current_mutex;
    
    if(output == NULL ||
       recall_id == NULL){
      return;
    }

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    mutex = ags_mutex_manager_lookup(mutex_manager,
				     channel);
    
    pthread_mutex_unlock(application_mutex);

    /* initialize audio */
    pthread_mutex_lock(mutex);

    audio = (AgsAudio *) output->audio;
    first_recycling = output->first_recycling;
    
    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   audio);
    
    pthread_mutex_unlock(application_mutex);

    /* lock context */
    //    ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));

    /* retrieve recall id */
    pthread_mutex_lock(mutex);

    flags = audio->flags; 

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      if(recall_id->recycling_context->parent != NULL){
	list = output->recall_id;
	
	while(list != NULL){
	  if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	    recall_id = list->data;
	    break;
	  }
	  
	  list = list->next;
	}
	
	if(list == NULL){
	  recall_id = NULL;
	}
      }
    }else{
      recall_id = ags_recall_id_find_recycling_context(output->recall_id,
						       recall_id->recycling_context);
    }

    pthread_mutex_unlock(mutex);
    
    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      pthread_mutex_lock(audio_mutex);
      
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }

      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));

      pthread_mutex_unlock(audio_mutex);
    }else{
      pthread_mutex_lock(audio_mutex);

      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;

      pthread_mutex_unlock(audio_mutex);
    }
    
    /* play output */
    ags_channel_play((GObject *) output,
		     recall_id,
		     stage);

    /* unlock context */
    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0){
      /* call function which play input */
      ags_channel_recursive_play_down_input(output,
					    default_recall_id);

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));
    
      /* play audio */
      ags_audio_play(audio,
		     audio_recall_id,
		     stage);

      ags_audio_play(audio,
		     default_recall_id,
		     stage);
    }else{
      /* call function which play input */
      ags_channel_recursive_play_down_input(output,
					    default_recall_id);

      /* lock context */
      //      ags_concurrent_tree_lock_context(AGS_CONCURRENT_TREE(first_recycling));

      /* play audio */
      ags_audio_play(audio,
		     audio_recall_id,
		     stage);
    }

    /* unlock context */
    //    ags_concurrent_tree_unlock_context(AGS_CONCURRENT_TREE(first_recycling));    
  }

  /* entry point */
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

  /* recursive compute audio */
  pthread_mutex_lock(mutex);

  link = channel->link;
  is_output = AGS_IS_OUTPUT(channel);

  pthread_mutex_unlock(mutex);

  if(is_output){
    ags_channel_recursive_play_down(channel,
				    recall_id);

    /*  */
    ags_channel_recursive_play_up(link,
				  recall_id);
  }else{
    AgsAudio *audio;
    AgsRecallID *audio_recall_id, *default_recall_id;

    guint flags;
    
    pthread_mutex_t *audio_mutex;

    pthread_mutex_lock(mutex);
    
    audio = (AgsAudio *) channel->audio;

    pthread_mutex_unlock(mutex);

    /* lookup mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
    
    pthread_mutex_unlock(application_mutex);

    /* get audio recall id */
    pthread_mutex_lock(audio_mutex);

    flags = audio->flags;
    
    audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							   recall_id->recycling_context);

    pthread_mutex_unlock(audio_mutex);

    /* get default recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
      default_recall_id = audio_recall_id;
    }else{
      default_recall_id = recall_id;
    }

    /* follow the links */
    if(link != NULL){
      ags_channel_recursive_play_down(link,
				      default_recall_id);
    }

    /*  */
    ags_channel_recursive_play_up(channel,
				  recall_id);
  }
}

/**
 * ags_channel_tillrecycling_cancel:
 * @channel: an #AgsChannel
 * @recall_id: an #AgsRecallID
 *
 * Traverses the tree down and up and calls ags_channel_cancel() for corresponding
 * @recall_id.
 *
 * Since: 0.4
 */
void
ags_channel_tillrecycling_cancel(AgsChannel *channel,
				 AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallID *audio_recall_id, *default_recall_id;

  auto void ags_channel_tillrecycling_cancel_up(AgsChannel *channel,
						AgsRecallID *recall_id);
  auto void ags_channel_tillrecycling_cancel_down_input(AgsChannel *output,
							AgsRecallID *default_recall_id);
  auto void ags_channel_tillrecycling_cancel_down(AgsChannel *output,
						  AgsRecallID *recall_id);

  void ags_channel_tillrecycling_cancel_up(AgsChannel *channel,
					   AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL){
      return;
    }
    
    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_cancel_up_OUTPUT;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

      ags_channel_cancel(current,
			 recall_id);

      /* AgsAudio */
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       recall_id->recycling_context);

      ags_audio_cancel(audio,
		       recall_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

      recall_id = ags_recall_id_find_recycling_context(current->recall_id,
						       recall_id->recycling_context);

    ags_channel_tillrecycling_cancel_up_OUTPUT:
      ags_channel_cancel(current,
			 recall_id);

      if(current == channel){
	AgsRecallID *audio_recall_id;

	/* AgsAudio */
	audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       recall_id->recycling_context);
	ags_audio_cancel(audio,
			 audio_recall_id);
      }
      
      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_tillrecycling_cancel_down_input(AgsChannel *output,
						   AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsRecallID *input_recall_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input,
				output->audio_channel);
          
      /* cancel recalls on input */
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							       default_recall_id->recycling_context);

	/* cancel input */
	ags_channel_cancel(input,
			   input_recall_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
								 default_recall_id->recycling_context);

	  /* follow the links */
	  ags_channel_tillrecycling_cancel_down(input->link,
						input_recall_id);
	}

	/* iterate */
	input = input->next_pad;
      }

    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      input_recall_id = ags_recall_id_find_recycling_context(input->recall_id,
							     default_recall_id->recycling_context);
      
      /* cancel recalls on input */
      ags_channel_cancel(input,
			 input_recall_id);

      /* follow the links */
      if(input->link != NULL){
	/* follow the links */
	ags_channel_tillrecycling_cancel_down(input->link,
					      input_recall_id);
      }
    }
  }
  void ags_channel_tillrecycling_cancel_down(AgsChannel *output,
					     AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id, *default_recall_id;
    GList *list;

    if(output == NULL || recall_id == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      if(recall_id->recycling_context->parent != NULL){
	list = output->recall_id;
	
	while(list != NULL){
	  if(AGS_RECALL_ID(list->data)->recycling_context->parent == recall_id->recycling_context){
	    recall_id = list->data;
	    break;
	  }
	  
	  list = list->next;
	}

	if(list == NULL){
	  recall_id = NULL;
	}
      }
    }else{
      recall_id = ags_recall_id_find_recycling_context(output->recall_id,
						       recall_id->recycling_context);
    }

    /* cancel output */
    ags_channel_cancel(output,
		       recall_id);

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsChannel *input;
      AgsRecyclingContext *recycling_context;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
      }

      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      if(audio_recall_id == NULL &&
	 AGS_RECYCLING_CONTEXT(recall_id->recycling_context)->parent == NULL){
	audio_recall_id = ags_recall_id_find_parent_recycling_context(audio->recall_id,
								      recall_id->recycling_context);
      }

      list = audio_recall_id->recycling_context->children;
      default_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							       AGS_RECYCLING_CONTEXT(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
							     recall_id->recycling_context);

      default_recall_id = audio_recall_id;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* call function which cancel input */
      ags_channel_tillrecycling_cancel_down_input(output,
						  default_recall_id);

      /* cancel audio */
      ags_audio_cancel(audio,
		       audio_recall_id);

      ags_audio_cancel(audio,
		       default_recall_id);
    }else{
      /* call function which cancel input */
      ags_channel_tillrecycling_cancel_down_input(output,
						  default_recall_id);

      /* cancel audio */
      ags_audio_cancel(audio,
		       audio_recall_id);
    }
  }

  /* entry point */
  audio = channel->audio;

  if(AGS_IS_OUTPUT(channel)){
    ags_channel_tillrecycling_cancel_down(channel,
					  recall_id);

    ags_channel_tillrecycling_cancel_up(channel->link,
					recall_id);
  }else{
    /* follow the links */
    if(channel->link != NULL){
      ags_channel_tillrecycling_cancel_down(channel->link,
					    recall_id);
    }
    
    ags_channel_tillrecycling_cancel_up(channel,
					recall_id);
  }
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
 * Since: 0.4
 */
void
ags_channel_recursive_reset_recall_ids(AgsChannel *channel, AgsChannel *link,
				       AgsChannel *old_channel_link, AgsChannel *old_link_link)
{
  AgsRecyclingContext *recycling_context;
  GList *link_devout_play_list, *channel_devout_play_list;
  GList *link_invalid_recall_id_list, *channel_invalid_recall_id_list;
  GList *channel_recall_id_list, *link_recall_id_list, *recycled_recall_id_list;
  GList *parent;
  GList *list;
  gint stage;

  /* applying functions */
  auto void ags_audio_reset_recall_id(AgsAudio *audio,
				      GList *recall_id_list, GList *devout_play_list,
				      GList *invalid_recall_id_list);
  auto void ags_channel_reset_recall_id(AgsChannel *channel,
					GList *recall_id_list, GList *devout_play_list,
					GList *invalid_recall_id_list);

  auto void ags_channel_recursive_reset_audio_duplicate_recall(AgsAudio *audio,
							       GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_duplicate_recall(AgsChannel *channel,
								 GList *recall_id_list);

  auto void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							     GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							       GList *recall_id_list);

  auto void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
							  GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
							    GList *recall_id_list);

  /* collectors of AgsPlayback and recall_id */
  auto GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								       GList *list, gboolean collect_recall_id);
  auto GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
								 GList *list, gboolean collect_recall_id);
  auto GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
							   gboolean collect_recall_id);
  auto GList* ags_channel_recursive_collect_recycled(AgsChannel *channel,
						     GList *collected_recall_id);
  
  /* tree iterator functions */
  auto void ags_channel_recursive_reset_recall_id_down_input(AgsChannel *output,
							     GList *recall_id_list, GList *devout_play_list,
							     GList *invalid_recall_id_list);
  auto void ags_channel_recursive_reset_recall_id_down(AgsChannel *current,
						       GList *recall_id_list, GList *devout_play_list,
						       GList *invalid_recall_id_list);
  auto void ags_channel_tillrecycling_reset_recall_id_up(AgsChannel *channel,
							 GList *recall_id_list, GList *devout_play_list,
							 GList *invalid_recall_id_list);

  auto void ags_channel_recursive_unset_recall_id_down_input(AgsChannel *output,
							     GList *invalid_recall_id_list);
  auto void ags_channel_recursive_unset_recall_id_down(AgsChannel *current,
						       GList *invalid_recall_id_list);
  auto void ags_channel_tillrecycling_unset_recall_id_up(AgsChannel *channel,
							 GList *invalid_recall_id_list);

  auto void ags_channel_recursive_duplicate_recall_down_input(AgsChannel *output,
							      GList *recall_id_list);
  auto void ags_channel_recursive_duplicate_recall_down(AgsChannel *channel,
							GList *recall_id_list);
  auto void ags_channel_tillrecycling_duplicate_recall_up(AgsChannel *channel,
							  GList *recall_id_list);

  auto void ags_channel_recursive_resolve_recall_down_input(AgsChannel *output,
							    GList *recall_id);
  auto void ags_channel_recursive_resolve_recall_down(AgsChannel *channel,
						      GList *recall_id);
  auto void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
							GList *recall_id);

  auto void ags_channel_recursive_init_recall_down_input(AgsChannel *output,
							 GList *recall_id);
  auto void ags_channel_recursive_init_recall_down(AgsChannel *channel,
						   GList *recall_id);
  auto void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						     GList *recall_id);

  /* implementation */
  void ags_audio_reset_recall_id(AgsAudio *audio,
				 GList *recall_id_list, GList *devout_play_list,
				 GList *invalid_recall_id_list){
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsPlayback *devout_play;
    GList *list;
    gboolean play;
    gboolean do_playback, do_sequencer, do_notation;

    AgsRecycling *recycling;
    gint recycling_length;
    gint i;
    gboolean success;
    
    while(invalid_recall_id_list != NULL){
      recall_id = AGS_RECALL_ID(invalid_recall_id_list->data);
      
      play = (recall_id->recycling_context->parent == NULL) ? TRUE: FALSE;
      list = play ? audio->play: audio->recall;

      /* remove AgsRecalls */
      while((list = ags_recall_find_recycling_context(list, G_OBJECT(recall_id->recycling_context))) != NULL){
	recall = AGS_RECALL(list->data);

#ifdef AGS_DEBUG
	g_message("reset recall id: invalidated\0");
#endif
	
	ags_audio_remove_recall(audio,
				(GObject *) recall,
				play);
	
	list = list->next;
      }
      
      /* remove AgsRecallID */
      ags_audio_remove_recall_id(audio, (GObject *) recall_id);

      /* iterate */
      invalid_recall_id_list = invalid_recall_id_list->next;
    }
    
    while(recall_id_list != NULL){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	success = FALSE;
      
	/* playback */
	if(devout_play_list != NULL){
	  devout_play = AGS_PLAYBACK(devout_play_list->data);
	  do_playback = ((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE;
	}else{
	  do_playback = (((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
	}

	if(do_playback){
	  success = TRUE;
	  
#ifdef AGS_DEBUG
	  g_message("recall id reset: play\0");
#endif

	  /* append new recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				   "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				   NULL);
	  ags_audio_add_recall_id(audio,
				  (GObject *) recall_id);
	
	  if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	  }

	  if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	  }

	  if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_NOTATION;
	  }
	
	  /* iterate */
	  recall_id_list = recall_id_list->next;

	  if(devout_play_list != NULL){
	    devout_play_list = devout_play_list->next;
	  }
	}

	if(recall_id_list == NULL){
	  break;
	}

	/* sequencer */
	devout_play = AGS_PLAYBACK(devout_play_list->data);
	
	if(devout_play_list != NULL){
	  devout_play = AGS_PLAYBACK(devout_play_list->data);
	  do_sequencer = ((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE;
	}else{
	  do_sequencer = (((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
	}
      
	if(do_sequencer){
	  success = TRUE;

#ifdef AGS_DEBUG
	  g_message("recall id reset: sequencer\0");
#endif

	  /* append new recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				   "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				   NULL);
	  ags_audio_add_recall_id(audio,
				  (GObject *) recall_id);

	  /* iterate */
	  recall_id_list = recall_id_list->next;

	  if(devout_play_list != NULL){
	    devout_play_list = devout_play_list->next;
	  }
	}

	if(recall_id_list == NULL){
	  break;
	}

	/* notation */
	if(devout_play_list != NULL){
	  devout_play = AGS_PLAYBACK(devout_play_list->data);
	  do_notation = ((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE;
	}else{
	  do_notation = (((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
	}
      
	if(do_notation){
	  success = TRUE;

#ifdef AGS_DEBUG
	  g_message("recall id reset: notation\0");
#endif

	  /* append new recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				   "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				   NULL);
	  ags_audio_add_recall_id(audio,
				  (GObject *) recall_id);

	  /* iterate */
	  recall_id_list = recall_id_list->next;

	  if(devout_play_list != NULL){
	    devout_play_list = devout_play_list->next;
	  }
	}

	if(recall_id_list == NULL){
	  break;
	}

	if(!success){
	  /* iterate */
	  recall_id_list = recall_id_list->next;

	  if(devout_play_list != NULL){
	    devout_play_list = devout_play_list->next;
	  }
	}
      }else{
	ags_audio_add_recall_id(audio,
				(GObject *) recall_id_list->data);
	  
	/* iterate */
	recall_id_list = recall_id_list->next;
	
	if(devout_play_list != NULL){
	  devout_play_list = devout_play_list->next;
	}
      }
    }
  }
  
  void ags_channel_reset_recall_id(AgsChannel *channel,
				   GList *recall_id_list, GList *devout_play_list,
				   GList *invalid_recall_id_list){
    AgsAudio *audio;
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsPlayback *devout_play;
    GList *list;
    gboolean play;
    gboolean do_playback, do_sequencer, do_notation;
    gboolean success;
    
    audio = channel->audio;
    
    while(invalid_recall_id_list != NULL){
      recall_id = AGS_RECALL_ID(invalid_recall_id_list->data);
      
      play = (recall_id->recycling_context->parent == NULL) ? TRUE: FALSE;
      list = play ? channel->play: channel->recall;

      /* unref AgsRecalls */
      while((list = ags_recall_find_recycling_context(list, G_OBJECT(recall_id->recycling_context))) != NULL){
	recall = AGS_RECALL(list->data);
	
	ags_channel_remove_recall(channel,
				  (GObject *) recall,
				  play);
	
	list = list->next;
      }
      
      /* unref AgsRecallID */
      ags_channel_remove_recall_id(channel,
				   recall_id);

      /* iterate */
      invalid_recall_id_list = invalid_recall_id_list->next;
    }
    
    while(recall_id_list != NULL){
      success = FALSE;
      
      /* playback */
      if(devout_play_list != NULL){
	devout_play = AGS_PLAYBACK(devout_play_list->data);
	do_playback = (((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE);
      }else{
	do_playback = (((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
      }
      
      if(do_playback){
	success = TRUE;
	
#ifdef AGS_DEBUG
	g_message("recall id reset: playback\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", channel->first_recycling,
				 "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				 NULL);
	ags_channel_add_recall_id(channel,
				  (GObject *) recall_id);
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	
	/* iterate */
	recall_id_list = recall_id_list->next;

	if(devout_play_list != NULL){
	  devout_play_list = devout_play_list->next;
	}
      }

      if(recall_id_list == NULL){
	break;
      }

      /* sequencer */
      if(devout_play_list != NULL){
	devout_play = AGS_PLAYBACK(devout_play_list->data);
	do_sequencer = (((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE);
      }else{
	do_sequencer = (((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
      }
      
      if(do_sequencer){
	success = TRUE;

#ifdef AGS_DEBUG
	g_message("recall id reset: sequencer\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", channel->first_recycling,
				 "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				 NULL);
	ags_channel_add_recall_id(channel,
				  (GObject *) recall_id);
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;

	/* iterate */
	recall_id_list = recall_id_list->next;

	if(devout_play_list != NULL){
	  devout_play_list = devout_play_list->next;
	}
      }

      if(recall_id_list == NULL){
	break;
      }

      /* notation */
      if(devout_play_list != NULL){
	devout_play = AGS_PLAYBACK(devout_play_list->data);
	do_notation = (((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(devout_play->flags)))) != 0) ? TRUE: FALSE);
      }else{
	do_notation = (((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0) ? TRUE: FALSE);
      }
      
      if(do_notation){
	success = TRUE;

#ifdef AGS_DEBUG
	g_message("recall id reset: notation\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", channel->first_recycling,
				 "recycling-context\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
				 NULL);
	ags_channel_add_recall_id(channel,
				  (GObject *) recall_id);
	recall_id->flags |= AGS_RECALL_ID_NOTATION;

	/* iterate */
	recall_id_list = recall_id_list->next;

	if(devout_play_list != NULL){
	  devout_play_list = devout_play_list->next;
	}
      }
      
      if(recall_id_list == NULL){
	break;
      }

      if(!success){
	recall_id_list = recall_id_list->next;
	
	if(devout_play_list != NULL){
	  devout_play_list = devout_play_list->next;
	}
      }
    }
  }

  void ags_channel_recursive_reset_audio_duplicate_recall(AgsAudio *audio,
							  GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_audio_duplicate_recall(audio,
				   recall_id);
      }
      
      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_channel_duplicate_recall(AgsChannel *channel,
							    GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(channel->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_channel_duplicate_recall(channel,
				     recall_id);
      }
            
      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_audio_resolve_recall(audio,
				 recall_id);
      }
      
      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							  GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(channel->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_channel_resolve_recall(channel,
				   recall_id);
      }
      
      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
						     GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(audio->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_audio_init_recall(audio, stage,
			      recall_id);
      }
      
      recall_id_list = recall_id_list->next;
    } 
  }

  void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
						       GList *recall_id_list){
    AgsAudio *audio;
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_context(channel->recall_id,
						       AGS_RECALL_ID(recall_id_list->data)->recycling_context);

      if(recall_id != NULL){
	ags_channel_init_recall(channel, stage,
				recall_id);
      }
      
      recall_id_list = recall_id_list->next;
    } 
  }

  GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								  GList *list, gboolean collect_recall_id){
    AgsAudio *audio;
    AgsChannel *current;

    if(output == NULL){
      return(list);
    }
    
    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* collect recall id and the recalls purpose */
	if(current->playback != NULL){
	  if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	     AGS_PLAYBACK(current->playback)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: play\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[0]: current->playback));
	  }

	  if((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	     AGS_PLAYBACK(current->playback)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: sequencer\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[1]: current->playback));
	  }

	  if((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	     AGS_PLAYBACK(current->playback)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: notation\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[2]: current->playback));
	  }
	}

	/* follow the links */
	list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								  list, collect_recall_id);

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);
      
      /* collect recall id and the recalls purpose */
      if(current->playback != NULL){
	if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[0]: current->playback));
	}

	if((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[1]: current->playback));
	}

	if((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[2]: current->playback));
	}
      }

      /* follow the links */
      list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								list, collect_recall_id);
    }

    return(list);
  }

  GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
							    GList *list, gboolean collect_recall_id){
    AgsAudio *audio;

    if(current == NULL){
      return(list);
    }

    audio = AGS_AUDIO(current->audio);

    /* collect devout play */
    if(current->playback != NULL){
      if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	 AGS_PLAYBACK(current->playback)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: play\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[0]: current->playback));
      }

      if((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	 AGS_PLAYBACK(current->playback)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: sequencer\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[1]: current->playback));
      }

      if((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	 AGS_PLAYBACK(current->playback)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: notation\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[2]: current->playback));
      }
    }
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    /* follow the links */
    list = ags_channel_tillrecycling_collect_devout_play_down_input(current,
								    list, collect_recall_id);

    return(list);
  }

  GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
						      gboolean collect_recall_id){
    AgsAudio *audio;
    AgsChannel *current;
    GList *list;

    if(channel == NULL){
      return(NULL);
    }

    current = channel;
    list = NULL;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_recursive_collect_devout_play_up_OUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      if(current->playback != NULL){
	if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[0]: current->playback));
	}

	if((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[1]: current->playback));
	}

	if((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[2]: current->playback));
	}
      }

      if(audio->output == NULL ||
	 (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }
      
      /* output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_recursive_collect_devout_play_up_OUTPUT:
      if(current->playback != NULL){
	if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[0]: current->playback));
	}

	if((AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[1]: current->playback));
	}

	if((AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(AGS_PLAYBACK(current->playback)->flags)))) != 0 &&
	   AGS_PLAYBACK(current->playback)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_PLAYBACK(current->playback)->recall_id[2]: current->playback));
	}
      }

      current = current->link;
    }

    return(list);
  }

  GList* ags_channel_recursive_collect_recycled(AgsChannel *channel,
						GList *collected_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *recall_id;
    GList *list;

    if(channel == NULL){
      return(NULL);
    }

    current = channel;
    list = NULL;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_recursive_collect_devout_play_up_OUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      /* nothing to do here */
      
      if(audio->output == NULL ||
	 (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }
      
      /* output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_recursive_collect_devout_play_up_OUTPUT:
      /* nothing to do here */
      
      current = current->link;
    }

    /* input */
    if(current != NULL &&
       AGS_IS_INPUT(current)){
      recall_id = current->recall_id;
    
      while(recall_id != NULL){
	if(g_list_find(collected_recall_id,
		       recall_id->data) == NULL){
	  list = g_list_prepend(list,
				recall_id->data);
	}
      
	recall_id = recall_id->next;
      }
    }

    return(list);    
  }
  
  void ags_channel_recursive_reset_recall_id_down_input(AgsChannel *output,
							GList *recall_id_list, GList *devout_play_list,
							GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id, *default_recall_id;
    
    GList *next_recall_id_list;
    GList *next_invalid_recall_id_list;
    
    audio = AGS_AUDIO(output->audio);
    
    /* apply */
    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);
      
      while(current != NULL){
	/* retrieve next recall id */
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  next_recall_id_list = recall_id_list;
	  next_invalid_recall_id_list = invalid_recall_id_list;
	}else{
	  next_recall_id_list = NULL;

	  while(recall_id_list != NULL){
	    default_recall_id = recall_id_list->data;
	    
	    /* recall id */
	    recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling\0", current->first_recycling,
				     "recycling-context\0", default_recall_id->recycling_context,
				     NULL);

	    if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	      recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	    }

	    if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	      recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	    }

	    if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	      recall_id->flags |= AGS_RECALL_ID_NOTATION;
	    }

	    next_recall_id_list = g_list_prepend(next_recall_id_list,
						 recall_id);
	
	    recall_id_list = recall_id_list->next;
	  }

	  next_recall_id_list = g_list_reverse(next_recall_id_list);
      
	  next_invalid_recall_id_list = NULL;

	  while(invalid_recall_id_list != NULL){
	    if(next_invalid_recall_id_list == NULL){
	      next_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	    }else{
	      next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							  ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	    }

	    invalid_recall_id_list = invalid_recall_id_list->next;
	  }
	}
	
	/* reset AgsInput */
	ags_channel_reset_recall_id(current,
				    recall_id_list, devout_play_list,
				    invalid_recall_id_list);
	
	/* follow the links */
	ags_channel_recursive_reset_recall_id_down(current->link,
						   next_recall_id_list, devout_play_list,
						   next_invalid_recall_id_list);

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);
      
      /* retrieve next recall id */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_recall_id_list = recall_id_list;
	next_invalid_recall_id_list = invalid_recall_id_list;
      }else{
	next_recall_id_list = NULL;

	while(recall_id_list != NULL){
	  default_recall_id = recall_id_list->data;
	  
	  /* recall id */
	  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				   "recycling\0", current->first_recycling,
				   "recycling-context\0", default_recall_id->recycling_context,
				   NULL);

	  if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	  }

	  if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	  }

	  if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	    recall_id->flags |= AGS_RECALL_ID_NOTATION;
	  }
	
	  next_recall_id_list = g_list_prepend(next_recall_id_list,
					       recall_id);
	
	  recall_id_list = recall_id_list->next;
	}

	next_recall_id_list = g_list_reverse(next_recall_id_list);
      
	next_invalid_recall_id_list = NULL;

	while(invalid_recall_id_list != NULL){
	  if(next_invalid_recall_id_list == NULL){
	    next_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	  }else{
	    next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	  }

	  invalid_recall_id_list = invalid_recall_id_list->next;
	}
      }
      
      /* reset AgsInput */
      ags_channel_reset_recall_id(current,
				  next_recall_id_list, devout_play_list,
				  next_invalid_recall_id_list);

      /* follow the links */
      ags_channel_recursive_reset_recall_id_down(current->link,
						 next_recall_id_list, devout_play_list,
						 next_invalid_recall_id_list);
    }
  }

  void ags_channel_recursive_reset_recall_id_down(AgsChannel *current,
						  GList *recall_id_list, GList *devout_play_list,
						  GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *input, *last_input;
    AgsRecycling *recycling;
    AgsRecyclingContext *recycling_context;
    AgsRecallID *recall_id;
    
    GList *output_recall_id_list, *default_recall_id_list;
    GList *output_invalid_recall_id_list, *default_invalid_recall_id_list;
    guint recycling_length;
    guint i;
    
    if(current == NULL){
      return;
    }

    audio = AGS_AUDIO(current->audio);

    /* input */
    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      current->audio_channel);
      last_input = ags_channel_pad_last(input);
    }else{
      input = ags_channel_nth(audio->input,
			      current->line);
      last_input = input;
    }

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      default_recall_id_list = 
	output_recall_id_list = recall_id_list;

      default_invalid_recall_id_list = 
	output_invalid_recall_id_list = invalid_recall_id_list;
    }else{
      output_recall_id_list = NULL;
      default_recall_id_list = NULL;

      while(recall_id_list != NULL){
	/* output related - create toplevel recycling container */
	recycling_length = ags_recycling_position(current->first_recycling, current->last_recycling->next,
						  current->last_recycling);
	recycling_length += 1;
	  
	recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
								 "length\0", recycling_length,
								 "parent\0", AGS_RECALL_ID(recall_id_list->data)->recycling_context,
								 NULL);
	ags_audio_add_recycling_context(audio,
					(GObject *) recycling_context);

	/* recycling */
	recycling = current->first_recycling;

	for(i = 0; i < recycling_length; i++){
	  recycling_context->recycling[i] = recycling;

	  recycling = recycling->next;
	}

	/* recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", current->first_recycling,
				 "recycling-context\0", recycling_context,
				 NULL);

	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}
	
	g_object_set(recycling_context,
		     "recall_id\0", recall_id,
		     NULL);

	output_recall_id_list = g_list_prepend(output_recall_id_list,
					       recall_id);

	/* default related - create toplevel recycling container */
	recycling_length = ags_recycling_position(input->first_recycling, last_input->last_recycling->next,
						  last_input->last_recycling);
	recycling_length += 1;
	  
	recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
								 "length\0", recycling_length,
								 "parent\0", AGS_RECALL_ID(output_recall_id_list->data)->recycling_context,
								 NULL);
	ags_audio_add_recycling_context(audio,
					(GObject *) recycling_context);

	/* recycling */
	recycling = input->first_recycling;

	for(i = 0; i < recycling_length; i++){
	  recycling_context->recycling[i] = recycling;

	  recycling = recycling->next;
	}

	/* recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", input->first_recycling,
				 "recycling-context\0", recycling_context,
				 NULL);

	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}
	
	g_object_set(recycling_context,
		     "recall_id\0", recall_id,
		     NULL);

	default_recall_id_list = g_list_prepend(default_recall_id_list,
						recall_id);

	/* iterate */
	recall_id_list = recall_id_list->next;
      }

      default_recall_id_list = g_list_reverse(default_recall_id_list);

      /* invalid recall id list */
      output_invalid_recall_id_list = NULL;
      default_invalid_recall_id_list = NULL;

      while(invalid_recall_id_list != NULL){
	if(output_invalid_recall_id_list == NULL){
	  output_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	  
	  default_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(output_invalid_recall_id_list->data)->recycling_context);
	}else{
	  output_invalid_recall_id_list = g_list_concat(output_invalid_recall_id_list,
							ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));

	  default_invalid_recall_id_list = g_list_concat(default_invalid_recall_id_list,
							 ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	}

	invalid_recall_id_list = invalid_recall_id_list->next;
      }
    }

    /* reset on AgsOutput */
    ags_channel_reset_recall_id(current,
				output_recall_id_list, devout_play_list,
				output_invalid_recall_id_list);

    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* reset on AgsAudio */
      ags_audio_reset_recall_id(audio,
				output_recall_id_list, devout_play_list,
				output_invalid_recall_id_list);
      
      /* reset new recall id on AgsAudio */
      ags_audio_reset_recall_id(audio,
				default_recall_id_list, devout_play_list,
				default_invalid_recall_id_list);
    }else{
      /* reset on AgsAudio */
      ags_audio_reset_recall_id(audio,
				output_recall_id_list, devout_play_list,
				output_invalid_recall_id_list);
    }

    /* follow the links */
    ags_channel_recursive_reset_recall_id_down_input(current,
						     default_recall_id_list, devout_play_list,
						     default_invalid_recall_id_list);
    
    /* free allocated lists */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_list_free(output_recall_id_list);
      g_list_free(default_recall_id_list);
      g_list_free(output_invalid_recall_id_list);
      g_list_free(default_invalid_recall_id_list);
    }
  }

  void ags_channel_tillrecycling_reset_recall_id_up(AgsChannel *channel,
						    GList *recall_id_list, GList *devout_play_list,
						    GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);
      
      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);

      goto ags_channel_tillrecycling_reset_recall_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_recall_id(current,
				  recall_id_list, devout_play_list,
				  invalid_recall_id_list);
      

      /* audio */
      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_reset_recall_id_upOUTPUT:
      ags_channel_reset_recall_id(current,
				  recall_id_list, devout_play_list,
				  invalid_recall_id_list);

      current = current->link;
    }
  }
  
  void ags_channel_recursive_unset_recall_id_down_input(AgsChannel *output,
							GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_invalid_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* reset AgsInput */
	ags_channel_reset_recall_id(current,
				    NULL, NULL,
				    invalid_recall_id_list);

	/* retrieve next recall id list */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  next_invalid_recall_id_list = invalid_recall_id_list;
	}else{
	  next_invalid_recall_id_list = NULL;

	  while(invalid_recall_id_list != NULL){
	    if(next_invalid_recall_id_list == NULL){
	      next_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	    }else{
	      next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							  ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	    }

	    invalid_recall_id_list = invalid_recall_id_list->next;
	  }
	}
    
	/* follow the links */
	ags_channel_recursive_unset_recall_id_down(current->link,
						   next_invalid_recall_id_list);

	/* free allocated lists */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  g_list_free(next_invalid_recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* reset AgsInput */
      ags_channel_reset_recall_id(current,
				  NULL, NULL,
				  invalid_recall_id_list);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_invalid_recall_id_list = invalid_recall_id_list;
      }else{
	next_invalid_recall_id_list = NULL;

	while(invalid_recall_id_list != NULL){
	  if(next_invalid_recall_id_list != NULL){
	    next_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	  }else{
	    next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	  }

	  invalid_recall_id_list = invalid_recall_id_list->next;
	}
      }
      
      /* follow the links */
      ags_channel_recursive_unset_recall_id_down(current->link,
						 next_invalid_recall_id_list);

      /* free allocated lists */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	g_list_free(next_invalid_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_unset_recall_id_down(AgsChannel *current,
						  GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    GList *next_invalid_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);
    
    /* reset on AgsOutput */
    ags_channel_reset_recall_id(current,
				NULL, NULL,
				invalid_recall_id_list);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_invalid_recall_id_list = invalid_recall_id_list;
    }else{
      next_invalid_recall_id_list = NULL;

      while(invalid_recall_id_list != NULL){
	if(next_invalid_recall_id_list){
	  next_invalid_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context);
	}else{
	  next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
						      ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list->data)->recycling_context));
	}

	invalid_recall_id_list = invalid_recall_id_list->next;
      }
    }

    /* reset on AgsAudio */
    ags_audio_reset_recall_id(audio,
			      NULL, NULL,
			      invalid_recall_id_list);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_audio_reset_recall_id(audio,
				NULL, NULL,
				invalid_recall_id_list);

      ags_audio_reset_recall_id(audio,
				NULL, NULL,
				next_invalid_recall_id_list);
    }else{
      ags_audio_reset_recall_id(audio,
				NULL, NULL,
				invalid_recall_id_list);
    }
    
    /* go down */
    ags_channel_recursive_unset_recall_id_down_input(current,
						     next_invalid_recall_id_list);

    /* free allocated lists */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_list_free(next_invalid_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_unset_recall_id_up(AgsChannel *channel,
						    GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_tillrecycling_reset_recall_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_recall_id(current,
				  NULL, NULL,
				  invalid_recall_id_list);


      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }      

      /* audio */
      ags_audio_reset_recall_id(audio,
				NULL, NULL,
				invalid_recall_id_list);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_reset_recall_id_upOUTPUT:

      ags_audio_reset_recall_id(audio,
				NULL, NULL,
				invalid_recall_id_list);


      ags_channel_reset_recall_id(current,
				  NULL, NULL,
				  invalid_recall_id_list);

      current = current->link;
    }
  }

  void ags_channel_recursive_duplicate_recall_down_input(AgsChannel *output,
							 GList *recall_id_list){

    AgsAudio *audio;
    AgsChannel *current;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* duplicate input */
	ags_channel_recursive_reset_channel_duplicate_recall(current,
							     recall_id_list);

	if(current->link != NULL){
	  /* follow the links */
	  ags_channel_recursive_duplicate_recall_down(current->link,
						      recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* duplicate input */
      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);

      /* follow the links */
      if(current->link != NULL){
	ags_channel_recursive_duplicate_recall_down(current->link,
						    recall_id_list);
      }
    }
  }
  void ags_channel_recursive_duplicate_recall_down(AgsChannel *current,
						   GList *recall_id_list){

    AgsAudio *audio;
    GList *next_recall_id_list, *default_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
      default_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;
      default_recall_id_list = NULL;
      
      while(recall_id_list != NULL){
	if(next_recall_id_list == NULL){
	  next_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context);

	  default_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context);
	}else{
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context));

	  default_recall_id_list = g_list_concat(default_recall_id_list,
						 ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context));
	}

	recall_id_list = recall_id_list->next;
      }
    }

    /* duplicate output */
    ags_channel_recursive_reset_channel_duplicate_recall(current,
							 next_recall_id_list);
            
    /* duplicate audio */
    ags_channel_recursive_reset_audio_duplicate_recall(audio,
						       next_recall_id_list);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_channel_recursive_reset_audio_duplicate_recall(audio,
							 default_recall_id_list);
    }

    /* go down */
    ags_channel_recursive_duplicate_recall_down_input(current,
						      default_recall_id_list);
    
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      g_list_free(next_recall_id_list);
      g_list_free(default_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_duplicate_recall_up(AgsChannel *channel,
						     GList *recall_id_list){
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_duplicate_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* duplicate input */
      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);      

      /* duplicate audio */
      ags_channel_recursive_reset_audio_duplicate_recall(audio,
							 recall_id_list);


      /* duplicate output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_duplicate_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);


      current = current->link;
    }
  }

  void ags_channel_recursive_resolve_recall_down_input(AgsChannel *output,
						       GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* resolve input */
	ags_channel_recursive_reset_channel_resolve_recall(current,
							   recall_id_list);

	if(current->link != NULL){
	  /* follow the links */
	  ags_channel_recursive_resolve_recall_down(current->link,
						    recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* resolve input */
      ags_channel_recursive_reset_channel_resolve_recall(current,
							 recall_id_list);

      /* follow the links */
      if(current->link != NULL){
	ags_channel_recursive_resolve_recall_down(current->link,
						  recall_id_list);
      }
    }
  }
  void ags_channel_recursive_resolve_recall_down(AgsChannel *current,
						 GList *recall_id_list)
  {
    AgsAudio *audio;
    GList *next_recall_id_list, *default_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
      default_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;
      default_recall_id_list = NULL;

      while(recall_id_list != NULL){
	if(next_recall_id_list != NULL){
	  next_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context);

	  default_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context);
	}else{
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context));

	  default_recall_id_list = g_list_concat(default_recall_id_list,
						 ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context));
	}

	recall_id_list = recall_id_list->next;
      }
    }

    /* resolve output */
    ags_channel_recursive_reset_channel_resolve_recall(current,
						       next_recall_id_list);
            
    /* resolve audio */
    ags_channel_recursive_reset_audio_resolve_recall(audio,
						     next_recall_id_list);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       default_recall_id_list);
    }

    /* go down */
    ags_channel_recursive_resolve_recall_down_input(current,
						    default_recall_id_list);
    
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      g_list_free(next_recall_id_list);
      g_list_free(default_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
						   GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_resolve_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* resolve input */
      ags_channel_recursive_reset_channel_resolve_recall(current,
							 recall_id_list);
      

      /* resolve audio */
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       recall_id_list);


      /* resolve output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_resolve_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_resolve_recall(current,
							 recall_id_list);


      current = current->link;
    }
  }
  void ags_channel_recursive_init_recall_down_input(AgsChannel *output,
						    GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){

	/* follow the links */
	ags_channel_recursive_init_recall_down(current->link,
					       recall_id_list);
	
	/* init AgsInput */
	ags_channel_recursive_reset_channel_init_recall(current,
							recall_id_list);

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);
      
      /* follow the links */
      ags_channel_recursive_init_recall_down(current->link,
					     recall_id_list);
      
      /* init AgsInput */
      if(current->link != NULL){
	ags_channel_recursive_reset_channel_init_recall(current,
							recall_id_list);
      }
    }
  }
  void ags_channel_recursive_init_recall_down(AgsChannel *current,
					      GList *recall_id_list)
  {
    AgsAudio *audio;
    GList *next_recall_id_list, *default_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
      default_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;
      default_recall_id_list = NULL;
      
      while(recall_id_list != NULL){
	if(next_recall_id_list == NULL){
	  next_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context);

	  default_recall_id_list = ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context);
	}else{
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(recall_id_list->data)->recycling_context));

	  default_recall_id_list = g_list_concat(default_recall_id_list,
						 ags_recycling_context_get_child_recall_id(AGS_RECALL_ID(next_recall_id_list->data)->recycling_context));
	}

	recall_id_list = recall_id_list->next;
      }
    }
    
    /* follow the links */
    ags_channel_recursive_resolve_recall_down_input(current,
						    default_recall_id_list);
    
    /* init audio */
    ags_channel_recursive_reset_audio_init_recall(audio,
						  next_recall_id_list);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_channel_recursive_reset_audio_init_recall(audio,
						    default_recall_id_list);
    }
    
    /* init output */
    ags_channel_recursive_reset_channel_init_recall(current,
						    next_recall_id_list);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_list_free(next_recall_id_list);
      g_list_free(default_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_init_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* init input */
      ags_channel_recursive_reset_channel_init_recall(current,
						      recall_id_list);
      
      /* init audio */
      ags_channel_recursive_reset_audio_init_recall(audio,
						    recall_id_list);


      /* init output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_init_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_init_recall(current,
						      recall_id_list);


      current = current->link;
    }

  }

  /* entry point */
  if(channel == NULL && link == NULL){
    return;
  }

  if((link != NULL && (AGS_IS_OUTPUT(link)) ||
      AGS_IS_INPUT(channel))){
    AgsChannel *tmp, *tmp_old;

    tmp = link;
    link = channel;
    channel = tmp;

    tmp_old = old_link_link;
    old_link_link = old_channel_link;
    old_channel_link = tmp_old;
  }

  /* collect AgsPlaybacks as lists */
  /* go down */
  channel_devout_play_list = NULL;
  channel_devout_play_list = ags_channel_tillrecycling_collect_devout_play_down(channel,
										channel_devout_play_list, FALSE);

  /* go up */
  link_devout_play_list = NULL;
  link_devout_play_list = ags_channel_recursive_collect_devout_play_up(link,
								       FALSE);

  /* retrieve lower */
  channel_recall_id_list = NULL;
  channel_recall_id_list = ags_channel_tillrecycling_collect_devout_play_down(channel,
									      channel_recall_id_list, TRUE);
  
  /* retrieve upper */
  link_recall_id_list = NULL;
  link_recall_id_list = ags_channel_recursive_collect_devout_play_up(link,
								     TRUE);

  recycled_recall_id_list = NULL;
  recycled_recall_id_list = ags_channel_recursive_collect_recycled(link,
								   link_recall_id_list);

  /* retrieve invalid lower */
  channel_invalid_recall_id_list = NULL;
  channel_invalid_recall_id_list = ags_channel_tillrecycling_collect_devout_play_down(old_link_link,
										      channel_invalid_recall_id_list,
										      TRUE);

  /* retrieve invalid upper */
  link_invalid_recall_id_list = NULL;
  link_invalid_recall_id_list = ags_channel_recursive_collect_devout_play_up(old_channel_link,
									     TRUE);

  /* go down */
  ags_channel_recursive_reset_recall_id_down(channel,
					     link_recall_id_list, link_devout_play_list,
					     channel_invalid_recall_id_list);

  ags_channel_recursive_reset_recall_id_down(channel,
  					     recycled_recall_id_list, NULL,
  					     NULL);

  /* go up */
  ags_channel_tillrecycling_reset_recall_id_up(link,
					       channel_recall_id_list, channel_devout_play_list,
					       link_invalid_recall_id_list);
  
  /* unset recall ids */
  /* go down */
  ags_channel_recursive_unset_recall_id_down(old_link_link,
					     link_invalid_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_unset_recall_id_up(old_channel_link,
  					       channel_invalid_recall_id_list);
  
  /* duplicate recalls */
  /* go down */
  ags_channel_recursive_duplicate_recall_down(channel,
					      link_recall_id_list);

  ags_channel_recursive_duplicate_recall_down(channel,
					      recycled_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_duplicate_recall_up(link,
						channel_recall_id_list);

  /* resolve recalls */
  /* go down */
  ags_channel_recursive_resolve_recall_down(channel,
					    link_recall_id_list);

  ags_channel_recursive_resolve_recall_down(channel,
					    recycled_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_resolve_recall_up(link,
					      channel_recall_id_list);

  /* init recalls */
  for(stage = 0; stage < 3; stage++){
    /* go down */
    ags_channel_recursive_init_recall_down(channel,
					   link_recall_id_list);

    ags_channel_recursive_init_recall_down(channel,
					   recycled_recall_id_list);

    /* go up */
    ags_channel_tillrecycling_init_recall_up(link,
					     channel_recall_id_list);

  }

  /* free the lists */
  g_list_free(channel_devout_play_list);
  g_list_free(link_devout_play_list);

  g_list_free(channel_invalid_recall_id_list);
  g_list_free(link_invalid_recall_id_list);

  g_list_free(channel_recall_id_list);
  g_list_free(link_recall_id_list);
}
 
/**
 * ags_channel_new:
 * @audio: the #AgsAudio
 *
 * Creates a #AgsChannel, linking tree to @audio.
 *
 * Returns: a new #AgsChannel
 *
 * Since: 0.3
 */
AgsChannel*
ags_channel_new(GObject *audio)
{
  AgsChannel *channel;

  channel = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
					"audio\0", audio,
					NULL);

  return(channel);
}
