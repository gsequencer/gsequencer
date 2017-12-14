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

#include <ags/audio/task/ags_open_file.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/i18n.h>

void ags_open_file_class_init(AgsOpenFileClass *open_file);
void ags_open_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_file_init(AgsOpenFile *open_file);
void ags_open_file_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_open_file_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_open_file_connect(AgsConnectable *connectable);
void ags_open_file_disconnect(AgsConnectable *connectable);
void ags_open_file_dispose(GObject *gobject);
void ags_open_file_finalize(GObject *gobject);
void ags_open_file_launch(AgsTask *task);

/**
 * SECTION:ags_open_file
 * @short_description: open file task
 * @title: AgsOpenFile
 * @section_id:
 * @include: ags/audio/task/ags_open_file.h
 *
 * The #AgsOpenFile task opens files.
 */

static gpointer ags_open_file_parent_class = NULL;
static AgsConnectableInterface *ags_open_file_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAMES,
  PROP_OVERWRITE_CHANNELS,
  PROP_CREATE_CHANNELS,
};

GType
ags_open_file_get_type()
{
  static GType ags_type_open_file = 0;

  if(!ags_type_open_file){
    static const GTypeInfo ags_open_file_info = {
      sizeof (AgsOpenFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOpenFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_open_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_open_file = g_type_register_static(AGS_TYPE_TASK,
						"AgsOpenFile",
						&ags_open_file_info,
						0);

    g_type_add_interface_static(ags_type_open_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_open_file);
}

void
ags_open_file_class_init(AgsOpenFileClass *open_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_open_file_parent_class = g_type_class_peek_parent(open_file);

  /* GObject */
  gobject = (GObjectClass *) open_file;

  gobject->set_property = ags_open_file_set_property;
  gobject->get_property = ags_open_file_get_property;

  gobject->dispose = ags_open_file_dispose;
  gobject->finalize = ags_open_file_finalize;

  /* properties */
  /**
   * AgsOpenFile:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of open file"),
				   i18n_pspec("The audio of open file task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);
  
  /**
   * AgsOpenFile:filenames:
   *
   * The assigned #GSList-struct providing filenames as string
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("filenames",
				    i18n_pspec("filenames of open file"),
				    i18n_pspec("The filenames of open file task"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAMES,
				  param_spec);
  
  /**
   * AgsOpenFile:overwrite-channels:
   *
   * As open files overwrite #AgsChannel.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("overwrite-channels",
				     i18n_pspec("open file overwriting channels"),
				     i18n_pspec("Do overwrite channels as open files"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OVERWRITE_CHANNELS,
				  param_spec);

  /**
   * AgsOpenFile:create-channels:
   *
   * As open files create #AgsChannel.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("create-channels",
				     i18n_pspec("open file creating channels"),
				     i18n_pspec("Do create channels as open files"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CREATE_CHANNELS,
				  param_spec);

  /* AgsTask */
  task = (AgsTaskClass *) open_file;

  task->launch = ags_open_file_launch;
}

void
ags_open_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_open_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_open_file_connect;
  connectable->disconnect = ags_open_file_disconnect;
}

void
ags_open_file_init(AgsOpenFile *open_file)
{
  open_file->audio = NULL;
  open_file->filenames = NULL;
  open_file->overwrite_channels = FALSE;
  open_file->create_channels = FALSE;
}

void
ags_open_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(open_file->audio == (GObject *) audio){
	return;
      }

      if(open_file->audio != NULL){
	g_object_unref(open_file->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      open_file->audio = (GObject *) audio;
    }
    break;
  case PROP_FILENAMES:
    {
      GSList *filenames;
      
      filenames = g_value_get_pointer(value);

      if(open_file->filenames == filenames){
	return;
      }

      if(open_file->filenames != NULL){
	g_slist_free(open_file->filenames);
      }
      
      open_file->filenames = g_slist_copy(filenames);
    }
    break;
  case PROP_OVERWRITE_CHANNELS:
    {
      open_file->overwrite_channels = g_value_get_boolean(value);
    }
    break;
  case PROP_CREATE_CHANNELS:
    {
      open_file->create_channels = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_file_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, open_file->audio);
    }
    break;
  case PROP_FILENAMES:
    {
      g_value_set_pointer(value,
			  g_slist_copy(open_file->filenames));
    }
    break;
  case PROP_OVERWRITE_CHANNELS:
    {
      g_value_set_boolean(value, open_file->overwrite_channels);
    }
    break;
  case PROP_CREATE_CHANNELS:
    {
      g_value_set_boolean(value, open_file->create_channels);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_file_connect(AgsConnectable *connectable)
{
  ags_open_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_open_file_disconnect(AgsConnectable *connectable)
{
  ags_open_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_open_file_dispose(GObject *gobject)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  if(open_file->audio != NULL){
    g_object_unref(open_file->audio);

    open_file->audio = NULL;
  }

  if(open_file->filenames != NULL){
    g_slist_free(open_file->filenames);

    open_file->filenames = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_file_parent_class)->dispose(gobject);
}

void
ags_open_file_finalize(GObject *gobject)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  if(open_file->audio != NULL){
    g_object_unref(open_file->audio);
  }

  if(open_file->filenames != NULL){
    g_slist_free(open_file->filenames);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_open_file_parent_class)->finalize(gobject);
}

void
ags_open_file_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsChannel *channel, *iter, *next_pad;
  AgsChannel *link;
  AgsRecycling *first_recycling;
  AgsAudioFile *audio_file;

  AgsOpenFile *open_file;

  AgsMutexManager *mutex_manager;
  
  AgsFileLink *file_link;

  GObject *soundcard;
  
  GSList *current;
  GList *audio_signal;
  
  gchar *current_filename;
  
  guint input_pads, pads_old;
  guint i, i_stop;
  guint j;
  
  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *iter_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  open_file = AGS_OPEN_FILE(task);

  audio = open_file->audio;

  current = open_file->filenames;

  i_stop = 0;
  
  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  input_pads = audio->input_pads;
  pads_old = audio->input_pads;
  
  if(open_file->overwrite_channels){
    channel = audio->input;

    i_stop = audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  /*  */
  if(open_file->create_channels){
    GList *list;

    i_stop = g_slist_length(open_file->filenames);
    
    if(open_file->overwrite_channels){
      if(i_stop > input_pads){
	ags_audio_set_pads(audio, AGS_TYPE_INPUT,
			   i_stop);
      }

      /* get some fields */
      pthread_mutex_lock(audio_mutex);
  
      channel = audio->input;

      input_pads = audio->input_pads;

      pthread_mutex_unlock(audio_mutex);
    }else{
      ags_audio_set_pads(audio, AGS_TYPE_INPUT,
			 audio->input_pads + i_stop);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);
  
      channel = audio->input;

      input_pads = audio->input_pads;

      pthread_mutex_unlock(audio_mutex);

      /* reset channel */
      channel = ags_channel_pad_nth(channel,
				    pads_old);
    }

    iter = ags_channel_pad_nth(audio->input,
			       pads_old);

    while(iter != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      iter_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) iter);
  
      pthread_mutex_unlock(application_mutex);

      /* connect */
      pthread_mutex_lock(iter_mutex);
      
      ags_connectable_connect(AGS_CONNECTABLE(iter));

      iter = iter->next;

      pthread_mutex_unlock(iter_mutex);
    }
  }

  for(i = 0; i < i_stop && current != NULL; i++){
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) cannel);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(channel_mutex);

    soundcard = channel->soundcard;

    next_pad = channel->next_pad;
    
    pthread_mutex_unlock(channel_mutex);

    /* audio file */
    current_filename = (gchar *) current->data;
    
    audio_file = ags_audio_file_new((gchar *) current_filename,
				    soundcard,
				    0, open_file->audio->audio_channels);

    if(!ags_audio_file_open(audio_file)){
      g_message("unable to open file - %s", current_filename);
      
      current = current->next;

      continue;
    }
    
    ags_audio_file_read_audio_signal(audio_file);

    iter = channel;
    audio_signal = audio_file->audio_signal;
    j = 0;

    /* connect */    
    while(iter != next_pad && audio_signal != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      iter_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) iter);
  
      pthread_mutex_unlock(application_mutex);

      /* set link */
      pthread_mutex_lock(iter_mutex);
      
      link = iter->link;

      pthread_mutex_unlock(iter_mutex);
      
      if(link != NULL){
	error = NULL;

	ags_channel_set_link(iter, NULL,
			     &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	}
      }

      /* file link */
      if(AGS_IS_INPUT(iter)){
	pthread_mutex_lock(iter_mutex);

	file_link = AGS_INPUT(channel)->file_link;
	
	if(file_link == NULL){
	  file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
				   NULL);
	
	  g_object_set(G_OBJECT(iter),
		       "file-link", file_link,
		       NULL);
	}

	g_object_set(file_link,
		     "filename", current_filename,
		     "preset", NULL,
		     "instrument", NULL,
		     "sample", NULL,
		     "audio-channel", j,
		     NULL);

	pthread_mutex_unlock(iter_mutex);
      }
      
      /* add as template */
      pthread_mutex_lock(iter_mutex);

      first_recycling = iter->first_recycling;

      pthread_mutex_unlock(iter_mutex);
      
      AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(first_recycling,
				     AGS_AUDIO_SIGNAL(audio_signal->data));

      /* iterate - audio channel and audio signal*/
      pthread_mutex_lock(iter_mutex);

      iter = iter->next;

      pthread_mutex_unlock(iter_mutex);

      audio_signal = audio_signal->next;
      
      j++;
    }

    /* iterate - pad and filename */
    pthread_mutex_lock(channel_mutex);
    
    channel = channel->next_pad;

    pthread_mutex_unlock(channel_mutex);

    current = current->next;
  }
}

/**
 * ags_open_file_new:
 * @audio: the #AgsAudio
 * @filenames: the filenames to be opened
 * @overwrite_channels: reset existing #AgsInput
 * @create_channels: inistantiate new #AgsInput, if @overwrite_channell as needed
 * else for sure
 *
 * Creates an #AgsOpenFile.
 *
 * Returns: an new #AgsOpenFile.
 *
 * Since: 1.0.0
 */
AgsOpenFile*
ags_open_file_new(AgsAudio *audio,
		  GSList *filenames,
		  gboolean overwrite_channels,
		  gboolean create_channels)
{
  AgsOpenFile *open_file;

  open_file = (AgsOpenFile *) g_object_new(AGS_TYPE_OPEN_FILE,
					   "audio", audio,
					   "filenames", filenames,
					   "overwrite-channels", overwrite_channels,
					   "create-channels", create_channels,
					   NULL);

  return(open_file);
}
