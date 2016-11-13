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

#include <ags/audio/task/ags_reset_audio_connection.h>

#include <ags/object/ags_connection_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio_connection.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

void ags_reset_audio_connection_class_init(AgsResetAudioConnectionClass *reset_audio_connection);
void ags_reset_audio_connection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_reset_audio_connection_init(AgsResetAudioConnection *reset_audio_connection);
void ags_reset_audio_connection_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_reset_audio_connection_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_reset_audio_connection_connect(AgsConnectable *connectable);
void ags_reset_audio_connection_disconnect(AgsConnectable *connectable);
void ags_reset_audio_connection_finalize(GObject *gobject);

void ags_reset_audio_connection_launch(AgsTask *task);

/**
 * SECTION:ags_reset_audio_connection
 * @short_description: reset audio connection task
 * @title: AgsResetAudioConnection
 * @section_id:
 * @include: ags/audio/task/ags_reset_audio_connection.h
 *
 * The #AgsResetAudioConnection resets audio connection.
 */

static gpointer ags_reset_audio_connection_parent_class = NULL;
static AgsConnectableInterface *ags_reset_audio_connection_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_AUDIO,
  PROP_CHANNEL_TYPE,
  PROP_PAD,
  PROP_AUDIO_CHANNEL,
  PROP_MAPPED_LINE,
};

GType
ags_reset_audio_connection_get_type()
{
  static GType ags_type_reset_audio_connection = 0;

  if(!ags_type_reset_audio_connection){
    static const GTypeInfo ags_reset_audio_connection_info = {
      sizeof (AgsResetAudioConnectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_audio_connection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResetAudioConnection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_audio_connection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_reset_audio_connection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_reset_audio_connection = g_type_register_static(AGS_TYPE_TASK,
							     "AgsResetAudioConnection\0",
							     &ags_reset_audio_connection_info,
							     0);
    
    g_type_add_interface_static(ags_type_reset_audio_connection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_reset_audio_connection);
}

void
ags_reset_audio_connection_class_init(AgsResetAudioConnectionClass *reset_audio_connection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_audio_connection_parent_class = g_type_class_peek_parent(reset_audio_connection);

  /* gobject */
  gobject = (GObjectClass *) reset_audio_connection;

  gobject->set_property = ags_reset_audio_connection_set_property;
  gobject->get_property = ags_reset_audio_connection_get_property;

  gobject->finalize = ags_reset_audio_connection_finalize;

  /* properties */
  /**
   * AgsResetAudioConnection:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of reset audio connection\0",
				   "The soundcard of reset audio connection task\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsResetAudioConnection:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio\0",
				   "audio of reset audio connection\0",
				   "The audio of reset audio connection task\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsResetAudioConnection:channel-type:
   *
   * The connection's channel type.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_gtype("channel-type\0",
				  "channel type\0",
				  "The channel type of connection\0",
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  
  /**
   * AgsResetAudioConnection:pad:
   *
   * The nth pad of audio.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("pad\0",
				 "nth pad\0",
				 "The nth pad of audio\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);


  /**
   * AgsResetAudioConnection:audio-channel:
   *
   * The nth audio channel of audio.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("audio-channel\0",
				 "nth audio channel\0",
				 "The nth audio channel of audio\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /**
   * AgsResetAudioConnection:mapped-line:
   *
   * The nth mapped line of connection.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("mapped-line\0",
				 "nth mapped line\0",
				 "The nth mapped line of connection\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAPPED_LINE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) reset_audio_connection;

  task->launch = ags_reset_audio_connection_launch;
}

void
ags_reset_audio_connection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_reset_audio_connection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_reset_audio_connection_connect;
  connectable->disconnect = ags_reset_audio_connection_disconnect;
}

void
ags_reset_audio_connection_init(AgsResetAudioConnection *reset_audio_connection)
{
  reset_audio_connection->soundcard = NULL;

  reset_audio_connection->audio = NULL;
  reset_audio_connection->channel_type = G_TYPE_NONE;
  
  reset_audio_connection->pad = 0;
  reset_audio_connection->audio_channel = 0;

  reset_audio_connection->mapped_line = 0;
}

void
ags_reset_audio_connection_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsResetAudioConnection *reset_audio_connection;

  reset_audio_connection = AGS_RESET_AUDIO_CONNECTION(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(reset_audio_connection->soundcard == (GObject *) soundcard){
	return;
      }

      if(reset_audio_connection->soundcard != NULL){
	g_object_unref(reset_audio_connection->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      reset_audio_connection->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(reset_audio_connection->audio == (GObject *) audio){
	return;
      }

      if(reset_audio_connection->audio != NULL){
	g_object_unref(reset_audio_connection->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      reset_audio_connection->audio = (GObject *) audio;
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      reset_audio_connection->channel_type = g_value_get_gtype(value);
    }
    break;
  case PROP_PAD:
    {
      reset_audio_connection->pad = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      reset_audio_connection->audio_channel = g_value_get_uint(value);
    }
    break;
  case PROP_MAPPED_LINE:
    {
      reset_audio_connection->mapped_line = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_reset_audio_connection_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsResetAudioConnection *reset_audio_connection;

  reset_audio_connection = AGS_RESET_AUDIO_CONNECTION(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, reset_audio_connection->soundcard);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, reset_audio_connection->audio);
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value, reset_audio_connection->channel_type);
    }
    break;
  case PROP_PAD:
    {
      g_value_set_uint(value, reset_audio_connection->pad);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, reset_audio_connection->audio_channel);
    }
    break;
  case PROP_MAPPED_LINE:
    {
      g_value_set_uint(value, reset_audio_connection->mapped_line);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_reset_audio_connection_connect(AgsConnectable *connectable)
{
  ags_reset_audio_connection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_reset_audio_connection_disconnect(AgsConnectable *connectable)
{
  ags_reset_audio_connection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_reset_audio_connection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_reset_audio_connection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_reset_audio_connection_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsAudioConnection *audio_connection;
  AgsChannel *channel;
  
  AgsResetAudioConnection *reset_audio_connection;

  AgsConnectionManager *connection_manager;

  GParameter *parameter;
  GList *list;
  
  reset_audio_connection = AGS_RESET_AUDIO_CONNECTION(task);
  audio = reset_audio_connection->audio;

  connection_manager = ags_connection_manager_get_instance();
  
  list = audio->audio_connection;
	  
  while((list = ags_audio_connection_find(list,
					  AGS_TYPE_INPUT,
					  reset_audio_connection->pad,
					  reset_audio_connection->audio_channel)) != NULL){
    GObject *data_object;

    g_object_get(G_OBJECT(list->data),
		 "data-object\0", &data_object,
		 NULL);
	    
    if(AGS_IS_SOUNDCARD(data_object)){
      break;
    }

    list = list->next;
  }

  if(list != NULL){
    audio_connection = list->data;
  }else{
    audio_connection = g_object_new(AGS_TYPE_AUDIO_CONNECTION,
				    "data-object\0", reset_audio_connection->soundcard,
				    NULL);
    ags_audio_add_audio_connection(audio,
				   (GObject *) audio_connection);
    ags_connection_manager_add_connection(connection_manager,
					  (AgsConnection *) audio_connection);
  }

  g_object_set(audio_connection,
	       "audio\0", audio,
	       "channel-type\0", reset_audio_connection->channel_type,
	       "pad\0", reset_audio_connection->pad,
	       "audio-channel\0", reset_audio_connection->audio_channel,
	       "mapped-line\0", reset_audio_connection->mapped_line,
	       NULL);

  if(g_type_is_a(reset_audio_connection->channel_type,
		 AGS_TYPE_INPUT)){
    channel = ags_channel_nth(audio->input,
			      reset_audio_connection->pad *audio->audio_channels + reset_audio_connection->audio_channel);
  }else{
    channel = ags_channel_nth(audio->output,
			      reset_audio_connection->pad *audio->audio_channels + reset_audio_connection->audio_channel);
  }

  parameter = g_new0(GParameter,
		     1);

  parameter[0].name = "soundcard\0";

  g_value_init(&(parameter[0].value),
	       G_TYPE_OBJECT);
  g_value_set_object(&(parameter[0].value),
		     reset_audio_connection->soundcard);
  
  if(channel != NULL){
    ags_channel_recursive_set_property(channel,
				       parameter, 1);
  }
}
  
/**
 * ags_reset_audio_connection_new:
 * @soundcard: the data-object
 * @audio: the #AgsAudio
 * @channel_type: the #GType
 * @pad: the pad
 * @audio_channel: the channel
 * @mapped_line: the mapped-line
 *
 * Creates an #AgsResetAudioConnection.
 *
 * Returns: an new #AgsResetAudioConnection.
 *
 * Since: 0.7.65
 */
AgsResetAudioConnection*
ags_reset_audio_connection_new(GObject *soundcard,
			       AgsAudio *audio,
			       GType channel_type,
			       guint pad,
			       guint audio_channel,
			       guint mapped_line)
{
  AgsResetAudioConnection *reset_audio_connection;

  reset_audio_connection = (AgsResetAudioConnection *) g_object_new(AGS_TYPE_RESET_AUDIO_CONNECTION,
						 NULL);

  reset_audio_connection->soundcard = soundcard;

  reset_audio_connection->audio = audio;
  reset_audio_connection->channel_type = channel_type;
  
  reset_audio_connection->pad = pad;
  reset_audio_connection->audio_channel = audio_channel;

  reset_audio_connection->mapped_line = mapped_line;

  return(reset_audio_connection);
}
