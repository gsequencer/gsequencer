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

#include <ags/audio/task/ags_add_audio.h>

#include <ags-lib/object/ags_connectable.h>

void ags_add_audio_class_init(AgsAddAudioClass *add_audio);
void ags_add_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_audio_init(AgsAddAudio *add_audio);
void ags_add_audio_connect(AgsConnectable *connectable);
void ags_add_audio_disconnect(AgsConnectable *connectable);
void ags_add_audio_finalize(GObject *gobject);

void ags_add_audio_launch(AgsTask *task);

/**
 * SECTION:ags_add_audio
 * @short_description: add audio object to devout
 * @title: AgsAddAudio
 * @section_id:
 * @include: ags/audio/task/ags_add_audio.h
 *
 * The #AgsAddAudio task adds #AgsAudio to #AgsDevout.
 */

static gpointer ags_add_audio_parent_class = NULL;
static AgsConnectableInterface *ags_add_audio_parent_connectable_interface;

GType
ags_add_audio_get_type()
{
  static GType ags_type_add_audio = 0;

  if(!ags_type_add_audio){
    static const GTypeInfo ags_add_audio_info = {
      sizeof (AgsAddAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_add_audio = g_type_register_static(AGS_TYPE_TASK,
						"AgsAddAudio\0",
						&ags_add_audio_info,
						0);
    
    g_type_add_interface_static(ags_type_add_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_audio);
}

void
ags_add_audio_class_init(AgsAddAudioClass *add_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_audio_parent_class = g_type_class_peek_parent(add_audio);

  /* gobject */
  gobject = (GObjectClass *) add_audio;

  gobject->finalize = ags_add_audio_finalize;

  /* task */
  task = (AgsTaskClass *) add_audio;

  task->launch = ags_add_audio_launch;
}

void
ags_add_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_audio_connect;
  connectable->disconnect = ags_add_audio_disconnect;
}

void
ags_add_audio_init(AgsAddAudio *add_audio)
{
  add_audio->devout = NULL;
  add_audio->audio = NULL;
}

void
ags_add_audio_connect(AgsConnectable *connectable)
{
  ags_add_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_audio_disconnect(AgsConnectable *connectable)
{
  ags_add_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_audio_launch(AgsTask *task)
{
  AgsAddAudio *add_audio;

  add_audio = AGS_ADD_AUDIO(task);

  /* add audio */
  ags_devout_add_audio(add_audio->devout,
		       G_OBJECT(add_audio->audio));

  /* AgsAudio */
  ags_connectable_connect(AGS_CONNECTABLE(add_audio->audio));
}

/**
 * ags_add_audio_new:
 * @devout: the #AgsDevout
 * @audio: the #AgsAudio to add
 *
 * Creates an #AgsAddAudio.
 *
 * Returns: an new #AgsAddAudio.
 *
 * Since: 0.4
 */
AgsAddAudio*
ags_add_audio_new(AgsDevout *devout,
		  AgsAudio *audio)
{
  AgsAddAudio *add_audio;

  add_audio = (AgsAddAudio *) g_object_new(AGS_TYPE_ADD_AUDIO,
					   NULL);

  add_audio->devout = devout;
  add_audio->audio = audio;

  return(add_audio);
}
