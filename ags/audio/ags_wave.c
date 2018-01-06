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

#include <ags/audio/ags_wave.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_wave_class_init(AgsWaveClass *wave);
void ags_wave_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_init(AgsWave *wave);
void ags_wave_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_wave_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_wave_connect(AgsConnectable *connectable);
void ags_wave_disconnect(AgsConnectable *connectable);
void ags_wave_dispose(GObject *gobject);
void ags_wave_finalize(GObject *gobject);

/**
 * SECTION:ags_wave
 * @short_description: Wave class supporting selection and clipboard.
 * @title: AgsWave
 * @section_id:
 * @include: ags/audio/ags_wave.h
 *
 * #AgsWave acts as a container of #AgsBuffer.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_BUFFER,
  PROP_TIMESTAMP,
  PROP_FORMAT,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
};

static gpointer ags_wave_parent_class = NULL;

GType
ags_wave_get_type()
{
  static GType ags_type_wave = 0;

  if(!ags_type_wave){
    static const GTypeInfo ags_wave_info = {
      sizeof(AgsWaveClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_wave_class_init,
      NULL,
      NULL,
      sizeof(AgsWave),
      0,
      (GInstanceInitFunc) ags_wave_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave = g_type_register_static(G_TYPE_OBJECT,
					   "AgsWave",
					   &ags_wave_info,
					   0);

    g_type_add_interface_static(ags_type_wave,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_wave);
}

void 
ags_wave_class_init(AgsWaveClass *wave)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wave_parent_class = g_type_class_peek_parent(wave);

  gobject = (GObjectClass *) wave;

  gobject->set_property = ags_wave_set_property;
  gobject->get_property = ags_wave_get_property;

  gobject->dispose = ags_wave_dispose;
  gobject->finalize = ags_wave_finalize;

  /* properties */
  /**
   * AgsWave:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of wave"),
				   i18n_pspec("The audio of wave"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsWave:audio-channel:
   *
   * The effect's audio-channel.
   * 
   * Since: 1.4.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("audio-channel of effect"),
				  i18n_pspec("The numerical audio-channel of effect"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);  

  /**
   * AgsWave:buffer:
   *
   * The assigned #AgsBuffer
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_object("buffer",
				   i18n_pspec("buffer of wave"),
				   i18n_pspec("The buffer of wave"),
				   AGS_TYPE_BUFFER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);
  
  /**
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of pattern"),
				   i18n_pspec("The timestamp of pattern"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /**
   * AgsWave:format:
   *
   * The audio buffer's format.
   * 
   * Since: 1.4.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format of audio buffer"),
				  i18n_pspec("The format of audio buffer"),
				  0,
				  G_MAXUINT,
				  AGS_SOUNDCARD_DEFAULT_FORMAT,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsWave:samplerate:
   *
   * The audio buffer's samplerate.
   * 
   * Since: 1.4.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate of audio buffer"),
				  i18n_pspec("The samplerate of audio buffer"),
				  0,
				  G_MAXUINT,
				  AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsWave:buffer-size:
   *
   * The audio buffer's buffer size.
   * 
   * Since: 1.4.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size of audio buffer"),
				  i18n_pspec("The buffer size of audio buffer"),
				  0,
				  G_MAXUINT,
				  AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);
}

void
ags_wave_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_connect;
  connectable->disconnect = ags_wave_disconnect;
}

void
ags_wave_init(AgsWave *wave)
{
  wave->flags = 0;

  wave->timestamp = ags_timestamp_new();

  wave->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  wave->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  wave->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(wave->timestamp);
  
  wave->audio_channel = 0;
  wave->audio = NULL;

  wave->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  wave->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  wave->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  wave->buffer = NULL;

  wave->loop_start = 0.0;
  wave->loop_end = 0.0;
  wave->offset = 0.0;

  wave->selection = NULL;
}

void
ags_wave_connect(AgsConnectable *connectable)
{
  AgsWave *wave;

  GList *list;
  
  wave = AGS_WAVE(connectable);

  if((AGS_WAVE_CONNECTED & (wave->flags)) != 0){
    return;
  }

  wave->flags |= AGS_WAVE_CONNECTED;

  /* buffer */
  list = wave->buffer;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_wave_disconnect(AgsConnectable *connectable)
{
  AgsWave *wave;

  GList *list;

  wave = AGS_WAVE(connectable);

  if((AGS_WAVE_CONNECTED & (wave->flags)) == 0){
    return;
  }

  wave->flags &= (~AGS_WAVE_CONNECTED);

  /* buffer */
  list = wave->buffer;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_wave_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  wave = AGS_WAVE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(wave->audio == (GObject *) audio){
	return;
      }

      if(wave->audio != NULL){
	g_object_unref(wave->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      wave->audio = (GObject *) audio;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      wave->audio_channel = audio_channel;
    }
    break;
  case PROP_BUFFER:
    {
      AgsBuffer *buffer;

      buffer = (AgsBuffer *) g_value_get_object(value);

      if(buffer == NULL ||
	 g_list_find(wave->buffer, buffer) != NULL){
	return;
      }

      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      if(timestamp == (AgsTimestamp *) wave->timestamp){
	return;
      }

      if(wave->timestamp != NULL){
	g_object_unref(G_OBJECT(wave->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      wave->timestamp = (GObject *) timestamp;
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      wave->format = format;
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      wave->samplerate = samplerate;
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      wave->buffer_size = buffer_size;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  wave = AGS_WAVE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, wave->audio);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, wave->audio_channel);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, wave->timestamp);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, wave->format);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, wave->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, wave->buffer_size);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_dispose(GObject *gobject)
{
  AgsWave *wave;

  GList *list;
  
  wave = AGS_WAVE(gobject);

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);

    wave->timestamp = NULL;
  }

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);

    wave->audio = NULL;
  }

    
  /* buffer and selection */
  list = wave->buffer;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(wave->buffer,
		   g_object_unref);

  g_list_free(wave->selection);

  wave->buffer = NULL;
  wave->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->dispose(gobject);
}

void
ags_wave_finalize(GObject *gobject)
{
  AgsWave *wave;

  wave = AGS_WAVE(gobject);

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);
  }

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);
  }
    
  /* buffer and selection */
  g_list_free_full(wave->buffer,
		   g_object_unref);

  g_list_free(wave->selection);
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->finalize(gobject);
}

GList*
ags_wave_find_near_timestamp(GList *wave, guint audio_channel,
			     AgsTimestamp *timestamp)
{
  //TODO:JK: implement me
  
  return(NULL);
}

GList*
ags_wave_add(GList *wave,
	     AgsWave *new_wave)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_wave_add_buffer(AgsWave *wave,
		    AgsBuffer *buffer,
		    gboolean use_selection_list)
{
  //TODO:JK: implement me
}

GList*
ags_wave_get_selection(AgsWave *wave)
{
  //TODO:JK: implement me
  
  return(NULL);
}

gboolean
ags_wave_is_buffer_selected(AgsWave *wave, AgsBuffer *buffer)
{
  //TODO:JK: implement me

  return(FALSE);
}

void
ags_wave_free_selection(AgsWave *wave)
{
  //TODO:JK: implement me
}

void
ags_wave_add_region_to_selection(AgsWave *wave,
				 guint64 x0, guint64 x1,
				 gboolean replace_current_selection)
{
  //TODO:JK: implement me
}

void
ags_wave_remove_region_from_selection(AgsWave *wave,
				      guint64 x0, guint64 x1)
{
  //TODO:JK: implement me
}

void
ags_wave_add_all_to_selection(AgsWave *wave)
{
  //TODO:JK: implement me
}

xmlNode*
ags_wave_copy_selection(AgsWave *wave)
{
  //TODO:JK: implement me

  return(NULL);
}

xmlNode*
ags_wave_cut_selection(AgsWave *wave)
{
  //TODO:JK: implement me
  
  return(NULL);
}

void
ags_wave_insert_from_clipboard(AgsWave *wave,
			       xmlNode *wave_node,
			       gboolean reset_x_offset, guint x_offset)
{
  //TODO:JK: implement me
}

void
ags_wave_insert_from_clipboard_extended(AgsWave *wave,
					xmlNode *wave_node,
					gboolean reset_x_offset, guint x_offset,
					gboolean match_audio_channel, gboolean no_duplicates)
{
  //TODO:JK: implement me
}

/**
 * ags_wave_new:
 * @audio: the assigned #AgsAudio
 * @audio_channel: the audio channel to be used
 *
 * Creates a #AgsWave, assigned to @audio_channel.
 *
 * Returns: a new #AgsWave
 *
 * Since: 1.4.0
 */
AgsWave*
ags_wave_new(GObject *audio,
	     guint audio_channel)
{
  AgsWave *wave;
  
  wave = (AgsWave *) g_object_new(AGS_TYPE_WAVE,
				  "audio", audio,
				  "audio-channel", audio_channel,
				  NULL);
  
  return(wave);
}
