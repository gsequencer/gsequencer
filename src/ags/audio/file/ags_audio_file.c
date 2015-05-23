/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/file/ags_audio_file.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/file/ags_playable.h>
#include <ags/audio/file/ags_sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
#include <string.h>

void ags_audio_file_class_init(AgsAudioFileClass *audio_file);
void ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_file_init(AgsAudioFile *audio_file);
void ags_audio_file_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_audio_file_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_audio_file_connect(AgsConnectable *connectable);
void ags_audio_file_disconnect(AgsConnectable *connectable);
void ags_audio_file_finalize(GObject *object);

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_START_CHANNEL,
  PROP_AUDIO_CHANNELS,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_PLAYABLE,
  PROP_AUDIO_SIGNAL,
};

enum{
  READ_BUFFER,
  LAST_SIGNAL,
};

/**
 * SECTION:ags_audio_file
 * @short_description: Audio file input/output
 * @title: AgsAudioFile
 * @section_id:
 * @include: ags/audio/file/ags_audio_file.h
 *
 * #AgsAudioFile is the base object to read/write audio data.
 */

static gpointer ags_audio_file_parent_class = NULL;
static AgsConnectableInterface *ags_audio_file_parent_connectable_interface;
static guint signals[LAST_SIGNAL];

GType
ags_audio_file_get_type()
{
  static GType ags_type_audio_file = 0;

  if(!ags_type_audio_file){
    static const GTypeInfo ags_audio_file_info = {
      sizeof (AgsAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_file = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAudioFile\0",
						 &ags_audio_file_info,
						 0);

    g_type_add_interface_static(ags_type_audio_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_audio_file);
}

void
ags_audio_file_class_init(AgsAudioFileClass *audio_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_file_parent_class = g_type_class_peek_parent(audio_file);

  gobject = (GObjectClass *) audio_file;

  gobject->set_property = ags_audio_file_set_property;
  gobject->get_property = ags_audio_file_get_property;

  gobject->finalize = ags_audio_file_finalize;
  
  /* properties */
  /**
   * AgsAudioFile:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of audio file\0",
				   "The soundcard what audio file has it's presets\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);


  /**
   * AgsAudioFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("filename\0",
				   "filename of lv2 manager\0",
				   "The filename this lv2 manager is assigned to\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsAudioFile:start-channel:
   *
   * The audio file's offset to start reading from.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_uint("start-channel\0",
				 "start-channel is the start offset\0",
				 "The start-channel indicates what audio channel should be read\0",
				 0, G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_CHANNEL,
				  param_spec);


  /**
   * AgsAudioFile:audio-channels:
   *
   * The audio file's count of channels to be read.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_uint("audio-channels\0",
				 "read audio-channels of channels\0",
				 "The audio-channels indicates how many channels should be read\0",
				 0, G_MAXUINT,
				 1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsAudioFile:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_uint("samplerate\0",
				 "using samplerate\0",
				 "The samplerate to be used\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioFile:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_uint("buffer-size\0",
				 "using buffer size\0",
				 "The buffer size to be used\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudioFile:playable:
   *
   * The containing  #AgsAudioSignal.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("playable\0",
				   "containing playable\0",
				   "The playable it contains\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYABLE,
				  param_spec);

  /**
   * AgsAudioFile:audio-signal:
   *
   * The containing  #AgsAudioSignal.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("audio-signal\0",
				   "containing audio signal\0",
				   "The audio signal it contains\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);
}

void
ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_file_connect;
  connectable->disconnect = ags_audio_file_disconnect;
}

void
ags_audio_file_init(AgsAudioFile *audio_file)
{
  audio_file->soundcard = NULL;

  audio_file->filename = NULL;

  audio_file->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  audio_file->frames = 0;
  audio_file->channels = 2;
  audio_file->format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  audio_file->start_channel = 0;
  audio_file->audio_channels = 0;

  audio_file->playable = NULL;
  audio_file->audio_signal = NULL;
}

void
ags_audio_file_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioFile *audio_file;

  audio_file = AGS_AUDIO_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(audio_file->filename == filename){
	return;
      }
      
      if(audio_file->filename != NULL){
	g_free(audio_file->filename);
      }

      audio_file->filename = g_strdup(filename);
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_object(value);

      if(soundcard == ((GObject *) audio_file->soundcard))
	return;

      if(audio_file->soundcard != NULL)
	g_object_unref(audio_file->soundcard);

      if(soundcard != NULL)
	g_object_ref(G_OBJECT(soundcard));

      audio_file->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_START_CHANNEL:
    {
      audio_file->start_channel = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      audio_file->audio_channels = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioFile *audio_file;

  audio_file = AGS_AUDIO_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    g_value_set_string(value, audio_file->filename);
    break;
  case PROP_SOUNDCARD:
    g_value_set_string(value, audio_file->soundcard);
    break;
  case PROP_START_CHANNEL:
    g_value_set_uint(value, audio_file->start_channel);
    break;
  case PROP_AUDIO_CHANNELS:
    g_value_set_uint(value, audio_file->audio_channels);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_finalize(GObject *gobject)
{
  AgsAudioFile *audio_file;
  GList *list, *list_next;

  audio_file = AGS_AUDIO_FILE(gobject);

  /* AgsAudioSignal */
  list = audio_file->audio_signal;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);
    
    list = list_next;
  }

  /* file */
  g_object_unref(audio_file->playable);

  G_OBJECT_CLASS(ags_audio_file_parent_class)->finalize(gobject);
}

void
ags_audio_file_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_audio_file_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

/**
 * ags_audio_file_open:
 * @audio_file: the #AgsAudioFile
 *
 * Open the #AgsAudioFile in read mode.
 *
 * Since: 0.3
 */
gboolean
ags_audio_file_open(AgsAudioFile *audio_file)
{
#ifdef AGS_DEBUG
  g_message("ags_audio_file_open: %s\0", audio_file->filename);
#endif

  if(g_file_test(audio_file->filename, G_FILE_TEST_EXISTS)){
    if(g_str_has_suffix(audio_file->filename, ".wav\0") ||
       g_str_has_suffix(audio_file->filename, ".ogg\0") ||
       g_str_has_suffix(audio_file->filename, ".flac\0")){
      GError *error;
      guint loop_start, loop_end;

      audio_file->playable = (GObject *) ags_sndfile_new();

      if(ags_playable_open(AGS_PLAYABLE(audio_file->playable),
			   audio_file->filename)){
	//FIXME:JK: this call should occure just before reading frames because of the new iterate functions of an AgsPlayable

	error = NULL;

	ags_playable_info(AGS_PLAYABLE(audio_file->playable),
			  &(audio_file->channels), &(audio_file->frames),
			  &loop_start, &loop_end,
			  &error);

	if(error != NULL){
	  g_error("%s\0", error->message);
	}

	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n\0");
      return(FALSE);
    }
  }
}

/**
 * ags_audio_file_rw_open:
 * @audio_file: the #AgsAudioFile
 * @create: create the file
 *
 * Open the #AgsAudioFile in read/write mode.
 *
 * Since: 0.4
 */
gboolean
ags_audio_file_rw_open(AgsAudioFile *audio_file,
		       gboolean create)
{
#ifdef AGS_DEBUG
  g_message("ags_audio_file_rw_open: %s\0", audio_file->filename);
#endif

  if(!create &&
     !g_file_test(audio_file->filename, G_FILE_TEST_EXISTS)){
    return(FALSE);
  }

  if(g_str_has_suffix(audio_file->filename, ".wav\0") ||
     g_str_has_suffix(audio_file->filename, ".ogg\0") ||
     g_str_has_suffix(audio_file->filename, ".flac\0")){
    GError *error;
    guint loop_start, loop_end;

    audio_file->playable = (GObject *) ags_sndfile_new();

    if(ags_playable_rw_open(AGS_PLAYABLE(audio_file->playable),
			    audio_file->filename,
			    create,
			    audio_file->samplerate, audio_file->channels,
			    audio_file->frames,
			    audio_file->format)){
      error = NULL;

      if(error != NULL){
	g_error("%s\0", error->message);
      }

      return(TRUE);
    }else{
      return(FALSE);
    }
  }else{
    g_message("ags_audio_file_open: unknown file type\n\0");
    return(FALSE);
  }
}

/**
 * ags_audio_file_open_from_data:
 * @audio_file: the #AgsAudioFile
 * @data: the audio data
 *
 * Open #AgsAudioFile using virtual functions.
 *
 * Since: 0.4
 */
gboolean
ags_audio_file_open_from_data(AgsAudioFile *audio_file, gchar *data)
{
#ifdef AGS_DEBUG
  g_message("ags_audio_file_open_from_data:\0");
#endif

  if(data != NULL){
    if(g_str_has_suffix(audio_file->filename, ".wav\0") ||
       g_str_has_suffix(audio_file->filename, ".ogg\0") ||
       g_str_has_suffix(audio_file->filename, ".flac\0")){
      GError *error;
      guint loop_start, loop_end;

      audio_file->playable = (GObject *) ags_sndfile_new();
      AGS_SNDFILE(audio_file->playable)->flags = AGS_SNDFILE_VIRTUAL;

      if(ags_playable_open(AGS_PLAYABLE(audio_file->playable),
			   audio_file->filename)){
	AGS_SNDFILE(audio_file->playable)->pointer = g_base64_decode(data,
								     &(AGS_SNDFILE(audio_file->playable)->length));
	AGS_SNDFILE(audio_file->playable)->current = AGS_SNDFILE(audio_file->playable)->pointer;

	error = NULL;

	ags_playable_info(AGS_PLAYABLE(audio_file->playable),
			  &(audio_file->channels), &(audio_file->frames),
			  &loop_start, &loop_end,
			  &error);

	if(error != NULL){
	  g_error("%s\0", error->message);
	}

	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n\0");
      return(FALSE);
    }
  }
}

/**
 * ags_audio_file_close:
 * @audio_file: the #AgsAudioFile
 *
 * Close the #AgsAudioFile.
 *
 * Since: 0.3
 */
void
ags_audio_file_close(AgsAudioFile *audio_file)
{
  ags_playable_close(AGS_PLAYABLE(audio_file->playable));
}

/**
 * ags_audio_file_read_audio_signal:
 * @audio_file: the #AgsAudioFile
 *
 * Convert the #AgsAudioFile to a #GList of buffers.
 *
 * Since: 0.3
 */
void
ags_audio_file_read_audio_signal(AgsAudioFile *audio_file)
{
  GList *list;

  list = ags_playable_read_audio_signal(AGS_PLAYABLE(audio_file->playable),
					audio_file->soundcard,
					audio_file->start_channel, audio_file->audio_channels);

  audio_file->audio_signal = list;
}

/**
 * ags_audio_file_seek:
 * @audio_file: the #AgsAudioFile
 * @frames: number of frames to seek
 * @whence: SEEK_SET, SEEK_CUR, or SEEK_END
 *
 * Position the #AgsAudioFile's internal data address.
 *
 * Since: 0.4
 */
void
ags_audio_file_seek(AgsAudioFile *audio_file, guint frames, gint whence)
{
  ags_playable_seek(AGS_PLAYABLE(audio_file->playable),
		    frames, whence);
}

/**
 * ags_audio_file_write:
 * @audio_file: the #AgsAudioFile
 * @buffer: the audio data
 * @buffer_size: the count of frames to write
 *
 * Write the buffer to #AgsAudioFile.
 *
 * Since: 0.4
 */
void
ags_audio_file_write(AgsAudioFile *audio_file,
		     signed short *buffer, guint buffer_size)
{
  ags_playable_write(AGS_PLAYABLE(audio_file->playable),
		     buffer, buffer_size);
}

/**
 * ags_audio_file_flush:
 * @audio_file: the #AgsAudioFile
 *
 * Flushes the #AgsAudioFile's internal buffer.
 *
 * Since: 0.4
 */
void
ags_audio_file_flush(AgsAudioFile *audio_file)
{
  ags_playable_flush(AGS_PLAYABLE(audio_file->playable));
}

/**
 * ags_audio_file_new:
 * @filename: the filename
 * @soundcard: defaults of #AgsSoundcard
 * @start_channel: ommited channels
 * @audio_channels: number of channels to read
 *
 * Creates an #AgsAudioFile.
 *
 * Returns: an empty #AgsAudioFile.
 *
 * Since: 0.3
 */
AgsAudioFile*
ags_audio_file_new(gchar *filename,
		   GObject *soundcard,
		   guint start_channel, guint audio_channels)
{
  AgsAudioFile *audio_file;

  audio_file = (AgsAudioFile *) g_object_new(AGS_TYPE_AUDIO_FILE,
					     "filename\0", filename,
					     "soundcard\0", soundcard,
					     "start-channel\0", start_channel,
					     "audio-channel\0", audio_channels,
					     NULL);

  return(audio_file);
}
