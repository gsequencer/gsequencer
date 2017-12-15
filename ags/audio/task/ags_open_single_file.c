/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/task/ags_open_single_file.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/i18n.h>

void ags_open_single_file_class_init(AgsOpenSingleFileClass *open_single_file);
void ags_open_single_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_single_file_init(AgsOpenSingleFile *open_single_file);
void ags_open_single_file_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_open_single_file_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_open_single_file_connect(AgsConnectable *connectable);
void ags_open_single_file_disconnect(AgsConnectable *connectable);
void ags_open_single_file_dispose(GObject *gobject);
void ags_open_single_file_finalize(GObject *gobject);
void ags_open_single_file_launch(AgsTask *task);

/**
 * SECTION:ags_open_single_file
 * @short_description: open single file task
 * @title: AgsOpenSingleFile
 * @section_id:
 * @include: ags/audio/task/ags_open_single_file.h
 *
 * The #AgsOpenFile task opens one single file.
 */

static gpointer ags_open_single_file_parent_class = NULL;
static AgsConnectableInterface *ags_open_single_file_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_START_CHANNEL,
  PROP_AUDIO_CHANNELS,
};

GType
ags_open_single_file_get_type()
{
  static GType ags_type_open_single_file = 0;

  if(!ags_type_open_single_file){
    static const GTypeInfo ags_open_single_file_info = {
      sizeof (AgsOpenSingleFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_single_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOpenSingleFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_single_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_open_single_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_open_single_file = g_type_register_static(AGS_TYPE_TASK,
						       "AgsOpenSingleFile",
						       &ags_open_single_file_info,
						       0);

    g_type_add_interface_static(ags_type_open_single_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_open_single_file);
}

void
ags_open_single_file_class_init(AgsOpenSingleFileClass *open_single_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_open_single_file_parent_class = g_type_class_peek_parent(open_single_file);

  /* GObject */
  gobject = (GObjectClass *) open_single_file;

  gobject->set_property = ags_open_single_file_set_property;
  gobject->get_property = ags_open_single_file_get_property;

  gobject->dispose = ags_open_single_file_dispose;
  gobject->finalize = ags_open_single_file_finalize;

  /* properties */
  /**
   * AgsOpenSingleFile:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of open file"),
				   i18n_pspec("The channel of open file task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsOpenSingleFile:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of open file"),
				   i18n_pspec("The soundcard of open file task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsOpenSingleFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename containing the output"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsOpenSingleFile:start-channel:
   *
   * The start channel of within pad @channel.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("start-channel",
				 i18n_pspec("start channel"),
				 i18n_pspec("The start channel"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_CHANNEL,
				  param_spec);

  /**
   * AgsOpenSingleFile:audio-channels:
   *
   * The audio channels count to read.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("audio channels"),
				 i18n_pspec("The audio channels to read"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /* AgsTask */
  task = (AgsTaskClass *) open_single_file;

  task->launch = ags_open_single_file_launch;
}

void
ags_open_single_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_open_single_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_open_single_file_connect;
  connectable->disconnect = ags_open_single_file_disconnect;
}

void
ags_open_single_file_init(AgsOpenSingleFile *open_single_file)
{
  open_single_file->channel = NULL;
  open_single_file->soundcard = NULL;
  open_single_file->filename = NULL;
  open_single_file->start_channel = 0;
  open_single_file->audio_channels = 0;
}

void
ags_open_single_file_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = AGS_OPEN_SINGLE_FILE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(open_single_file->channel == (GObject *) channel){
	return;
      }

      if(open_single_file->channel != NULL){
	g_object_unref(open_single_file->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      open_single_file->channel = (GObject *) channel;
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(open_single_file->soundcard == (GObject *) soundcard){
	return;
      }

      if(open_single_file->soundcard != NULL){
	g_object_unref(open_single_file->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      open_single_file->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(open_single_file->filename == filename){
	return;
      }

      if(open_single_file->filename != NULL){
        g_free(open_single_file->filename);
      }

      open_single_file->filename = g_strdup(filename);
    }
    break;
  case PROP_START_CHANNEL:
    {
      open_single_file->start_channel = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      open_single_file->audio_channels = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_single_file_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = AGS_OPEN_SINGLE_FILE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, open_single_file->channel);
    }
    break;
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, open_single_file->soundcard);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, open_single_file->filename);
    }
    break;
  case PROP_START_CHANNEL:
    {
      g_value_set_uint(value, open_single_file->start_channel);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, open_single_file->audio_channels);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_single_file_connect(AgsConnectable *connectable)
{
  ags_open_single_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_open_single_file_disconnect(AgsConnectable *connectable)
{
  ags_open_single_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_open_single_file_dispose(GObject *gobject)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = AGS_OPEN_SINGLE_FILE(gobject);

  if(open_single_file->channel != NULL){
    g_object_unref(open_single_file->channel);

    open_single_file->channel = NULL;
  }

  if(open_single_file->soundcard != NULL){
    g_object_unref(open_single_file->soundcard);

    open_single_file->soundcard = NULL;
  }

  if(open_single_file->filename != NULL){
    g_free(open_single_file->filename);

    open_single_file->filename = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_single_file_parent_class)->dispose(gobject);
}

void
ags_open_single_file_finalize(GObject *gobject)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = AGS_OPEN_SINGLE_FILE(gobject);

  if(open_single_file->channel != NULL){
    g_object_unref(open_single_file->channel);
  }

  if(open_single_file->soundcard != NULL){
    g_object_unref(open_single_file->soundcard);
  }

  if(open_single_file->filename != NULL){
    g_free(open_single_file->filename);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_open_single_file_parent_class)->finalize(gobject);
}

void
ags_open_single_file_launch(AgsTask *task)
{
  AgsOpenSingleFile *open_single_file;

  AgsChannel *channel;
  AgsChannel *link;
  AgsRecycling *first_recycling;
  
  AgsAudioFile *audio_file;

  AgsFileLink *file_link;

  AgsMutexManager *mutex_manager;

  GList *audio_signal;

  guint i;

  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  open_single_file = AGS_OPEN_SINGLE_FILE(task);

  channel = open_single_file->channel;

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* open audio file and read audio signal */
  audio_file = ags_audio_file_new(open_single_file->filename,
				  open_single_file->soundcard,
				  open_single_file->start_channel, open_single_file->audio_channels);

  if(!ags_audio_file_open(audio_file)){
    g_message("unable to open file - %s", open_single_file->filename);

    return;
  }
  
  ags_audio_file_read_audio_signal(audio_file);

  /* iterate channels */
  audio_signal = audio_file->audio_signal;

  for(i = 0; i < open_single_file->audio_channels && audio_signal != NULL; i++){
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);
    
    /* set link */
    pthread_mutex_lock(channel_mutex);

    link = channel->link;

    pthread_mutex_unlock(channel_mutex);

    if(link != NULL){
      error = NULL;
      ags_channel_set_link(channel, NULL,
			   &error);

      if(error != NULL){
	g_warning("%s", error->message);
      }
    }

    /* file link */
    if(AGS_IS_INPUT(channel)){
      pthread_mutex_lock(channel_mutex);

      file_link = AGS_INPUT(channel)->file_link;
    
      if(file_link == NULL){
	file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
				 NULL);
      
	g_object_set(channel,
		     "file-link", file_link,
		     NULL);
      }

      g_object_set(file_link,
		   "filename", open_single_file->filename,
		   "preset", NULL,
		   "instrument", NULL,
		   "sample", NULL,
		   "audio-channel", open_single_file->start_channel + i,
		   NULL);
      
      pthread_mutex_unlock(channel_mutex);
    }

    /* add as template */
    pthread_mutex_lock(channel_mutex);

    first_recycling = channel->first_recycling;

    pthread_mutex_unlock(channel_mutex);

    AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
    ags_recycling_add_audio_signal(first_recycling,
				   AGS_AUDIO_SIGNAL(audio_signal->data));

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);

    audio_signal = audio_signal->next;
  }

  g_object_unref(audio_file);
}

AgsOpenSingleFile*
ags_open_single_file_new(AgsChannel *channel,
			 GObject *soundcard,
			 gchar *filename,
			 guint start_channel,
			 guint audio_channels)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = (AgsOpenSingleFile *) g_object_new(AGS_TYPE_OPEN_SINGLE_FILE,
							"channel", channel,
							"soundcard", soundcard,
							"filename", filename,
							"start-channel", start_channel,
							"audio-channels", audio_channels,
							NULL);

  return(open_single_file);
}
