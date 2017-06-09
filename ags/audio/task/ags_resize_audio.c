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

#include <ags/audio/task/ags_resize_audio.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

void ags_resize_audio_class_init(AgsResizeAudioClass *resize_audio);
void ags_resize_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_resize_audio_init(AgsResizeAudio *resize_audio);
void ags_resize_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_resize_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_resize_audio_connect(AgsConnectable *connectable);
void ags_resize_audio_disconnect(AgsConnectable *connectable);
void ags_resize_audio_finalize(GObject *gobject);

void ags_resize_audio_launch(AgsTask *task);

/**
 * SECTION:ags_resize_audio
 * @short_description: resize audio task
 * @title: AgsResizeAudio
 * @section_id:
 * @include: ags/audio/task/ags_resize_audio.h
 *
 * The #AgsResizeAudio task resizes #AgsAudio.
 */

static gpointer ags_resize_audio_parent_class = NULL;
static AgsConnectableInterface *ags_resize_audio_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_OUTPUT_PADS,
  PROP_INPUT_PADS,
  PROP_AUDIO_CHANNELS,
};

GType
ags_resize_audio_get_type()
{
  static GType ags_type_resize_audio = 0;

  if(!ags_type_resize_audio){
    static const GTypeInfo ags_resize_audio_info = {
      sizeof (AgsResizeAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_resize_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResizeAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_resize_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_resize_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_resize_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsResizeAudio",
						   &ags_resize_audio_info,
						   0);
    
    g_type_add_interface_static(ags_type_resize_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_resize_audio);
}

void
ags_resize_audio_class_init(AgsResizeAudioClass *resize_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_resize_audio_parent_class = g_type_class_peek_parent(resize_audio);

  /* gobject */
  gobject = (GObjectClass *) resize_audio;

  gobject->set_property = ags_resize_audio_set_property;
  gobject->get_property = ags_resize_audio_get_property;

  gobject->finalize = ags_resize_audio_finalize;

  /* properties */
  /**
   * AgsResizeAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("audio",
				   "audio of resize audio",
				   "The audio of resize audio task",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsResizeAudio:output-pads:
   *
   * The count of output pads to apply to audio.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("output-pads",
				 "output pads",
				 "The count of output pads",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_PADS,
				  param_spec);

  /**
   * AgsResizeAudio:input-pads:
   *
   * The count of input pads to apply to audio.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("input-pads",
				 "input pads",
				 "The count of input pads",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_PADS,
				  param_spec);

  /**
   * AgsResizeAudio:audio-channels:
   *
   * The count of audio channels to apply to audio.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("audio-channels",
				 "audio channels",
				 "The count of audio channels",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);
  

  /* task */
  task = (AgsTaskClass *) resize_audio;

  task->launch = ags_resize_audio_launch;
}

void
ags_resize_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_resize_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_resize_audio_connect;
  connectable->disconnect = ags_resize_audio_disconnect;
}

void
ags_resize_audio_init(AgsResizeAudio *resize_audio)
{
  resize_audio->audio = NULL;
  resize_audio->output_pads = 0;
  resize_audio->input_pads = 0;
  resize_audio->audio_channels = 0;
}

void
ags_resize_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsResizeAudio *resize_audio;

  resize_audio = AGS_RESIZE_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(resize_audio->audio == (GObject *) audio){
	return;
      }

      if(resize_audio->audio != NULL){
	g_object_unref(resize_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      resize_audio->audio = (GObject *) audio;
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      resize_audio->output_pads = g_value_get_uint(value);
    }
    break;
  case PROP_INPUT_PADS:
    {
      resize_audio->input_pads = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      resize_audio->audio_channels = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_resize_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsResizeAudio *resize_audio;

  resize_audio = AGS_RESIZE_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, resize_audio->audio);
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      g_value_set_uint(value, resize_audio->output_pads);
    }
    break;
  case PROP_INPUT_PADS:
    {
      g_value_set_uint(value, resize_audio->input_pads);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, resize_audio->audio_channels);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_resize_audio_connect(AgsConnectable *connectable)
{
  ags_resize_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_resize_audio_disconnect(AgsConnectable *connectable)
{
  ags_resize_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_resize_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_resize_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_resize_audio_launch(AgsTask *task)
{
  AgsResizeAudio *resize_audio;
  AgsChannel *iter;
  guint pads_old;
  
  resize_audio = AGS_RESIZE_AUDIO(task);

  /* resize audio */
  if(resize_audio->audio->output_pads != resize_audio->output_pads){
    pads_old = resize_audio->audio->output_pads;
    
    ags_audio_set_pads(resize_audio->audio,
		       AGS_TYPE_OUTPUT,
		       resize_audio->output_pads);

    if(pads_old < resize_audio->audio->output_pads){
      iter = ags_channel_pad_nth(resize_audio->audio->input,
				 pads_old);
      
      while(iter != NULL){
	ags_connectable_connect(AGS_CONNECTABLE(iter));
	
	iter = iter->next;
      }
    }
  }

  if(resize_audio->audio->input_pads != resize_audio->input_pads){
    pads_old = resize_audio->audio->input_pads;

    ags_audio_set_pads(resize_audio->audio,
		       AGS_TYPE_INPUT,
		       resize_audio->input_pads);

    if(pads_old < resize_audio->audio->output_pads){
      iter = ags_channel_pad_nth(resize_audio->audio->input,
				 pads_old);
      
      while(iter != NULL){
	ags_connectable_connect(AGS_CONNECTABLE(iter));
	
	iter = iter->next;
      }
    }
  }

  if(resize_audio->audio->audio_channels != resize_audio->audio_channels){
    ags_audio_set_audio_channels(resize_audio->audio,
				 resize_audio->audio_channels);
  }
}
  
/**
 * ags_resize_audio_new:
 * @audio: the #AgsAudio to resize
 * @output_pads: output pads
 * @input_pads: input pads
 * @audio_channels: audio channels
 *
 * Creates an #AgsResizeAudio.
 *
 * Returns: an new #AgsResizeAudio.
 *
 * Since: 0.4
 */
AgsResizeAudio*
ags_resize_audio_new(AgsAudio *audio,
		     guint output_pads,
		     guint input_pads,
		     guint audio_channels)
{
  AgsResizeAudio *resize_audio;

  resize_audio = (AgsResizeAudio *) g_object_new(AGS_TYPE_RESIZE_AUDIO,
						 NULL);

  resize_audio->audio = audio;
  resize_audio->output_pads = output_pads;
  resize_audio->input_pads = input_pads;
  resize_audio->audio_channels = audio_channels;

  return(resize_audio);
}
