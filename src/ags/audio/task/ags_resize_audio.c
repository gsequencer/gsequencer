/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/task/ags_resize_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

void ags_resize_audio_class_init(AgsResizeAudioClass *resize_audio);
void ags_resize_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_resize_audio_init(AgsResizeAudio *resize_audio);
void ags_resize_audio_connect(AgsConnectable *connectable);
void ags_resize_audio_disconnect(AgsConnectable *connectable);
void ags_resize_audio_finalize(GObject *gobject);

void ags_resize_audio_launch(AgsTask *task);

/**
 * SECTION:ags_resize_audio
 * @short_description: resize audio object
 * @title: AgsResizeAudio
 * @section_id:
 * @include: ags/audio/task/ags_resize_audio.h
 *
 * The #AgsResizeAudio task resizes #AgsAudio.
 */

static gpointer ags_resize_audio_parent_class = NULL;
static AgsConnectableInterface *ags_resize_audio_parent_connectable_interface;

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
						   "AgsResizeAudio\0",
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

  ags_resize_audio_parent_class = g_type_class_peek_parent(resize_audio);

  /* gobject */
  gobject = (GObjectClass *) resize_audio;

  gobject->finalize = ags_resize_audio_finalize;

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

  resize_audio = AGS_RESIZE_AUDIO(task);

  /* resize audio */
  if(resize_audio->audio->output_pads != resize_audio->output_pads){
    ags_audio_set_pads(resize_audio->audio,
		       AGS_TYPE_OUTPUT,
		       resize_audio->output_pads);
  }

  if(resize_audio->audio->input_pads != resize_audio->input_pads){
    ags_audio_set_pads(resize_audio->audio,
		       AGS_TYPE_INPUT,
		       resize_audio->input_pads);
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
