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

#include <ags/audio/task/ags_set_audio_channels.h>

#include <ags-lib/object/ags_connectable.h>

void ags_set_audio_channels_class_init(AgsSetAudioChannelsClass *set_audio_channels);
void ags_set_audio_channels_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_audio_channels_init(AgsSetAudioChannels *set_audio_channels);
void ags_set_audio_channels_connect(AgsConnectable *connectable);
void ags_set_audio_channels_disconnect(AgsConnectable *connectable);
void ags_set_audio_channels_finalize(GObject *gobject);

void ags_set_audio_channels_launch(AgsTask *task);

/**
 * SECTION:ags_set_audio_channels
 * @short_description: resizes audio channels
 * @title: AgsSetAudioChannels
 * @section_id:
 * @include: ags/audio/task/ags_set_audio_channels.h
 *
 * The #AgsSetAudioChannels task resizes audio channels of #AgsDevout.
 */

static gpointer ags_set_audio_channels_parent_class = NULL;
static AgsConnectableInterface *ags_set_audio_channels_parent_connectable_interface;

GType
ags_set_audio_channels_get_type()
{
  static GType ags_type_set_audio_channels = 0;

  if(!ags_type_set_audio_channels){
    static const GTypeInfo ags_set_audio_channels_info = {
      sizeof (AgsSetAudioChannelsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_audio_channels_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetAudioChannels),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_audio_channels_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_audio_channels_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_audio_channels = g_type_register_static(AGS_TYPE_TASK,
							 "AgsSetAudioChannels\0",
							 &ags_set_audio_channels_info,
							 0);
    
    g_type_add_interface_static(ags_type_set_audio_channels,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_audio_channels);
}

void
ags_set_audio_channels_class_init(AgsSetAudioChannelsClass *set_audio_channels)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_set_audio_channels_parent_class = g_type_class_peek_parent(set_audio_channels);

  /* gobject */
  gobject = (GObjectClass *) set_audio_channels;

  gobject->finalize = ags_set_audio_channels_finalize;

  /* task */
  task = (AgsTaskClass *) set_audio_channels;

  task->launch = ags_set_audio_channels_launch;
}

void
ags_set_audio_channels_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_audio_channels_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_audio_channels_connect;
  connectable->disconnect = ags_set_audio_channels_disconnect;
}

void
ags_set_audio_channels_init(AgsSetAudioChannels *set_audio_channels)
{
  set_audio_channels->devout = NULL;
  set_audio_channels->audio_channels = 0;
}

void
ags_set_audio_channels_connect(AgsConnectable *connectable)
{
  ags_set_audio_channels_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_audio_channels_disconnect(AgsConnectable *connectable)
{
  ags_set_audio_channels_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_audio_channels_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_set_audio_channels_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_set_audio_channels_launch(AgsTask *task)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(task);

  g_object_set(G_OBJECT(set_audio_channels->devout),
	       "pcm_channels\0", set_audio_channels->audio_channels,
	       NULL);
}

/**
 * ags_set_audio_channels_new:
 * @devout: the #AgsDevout to reset
 * @audio_channels: the new count of audio channels
 *
 * Creates an #AgsSetAudioChannels.
 *
 * Returns: an new #AgsSetAudioChannels.
 *
 * Since: 0.4
 */
AgsSetAudioChannels*
ags_set_audio_channels_new(AgsDevout *devout, guint audio_channels)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = (AgsSetAudioChannels *) g_object_new(AGS_TYPE_SET_AUDIO_CHANNELS,
							    NULL);


  set_audio_channels->devout = devout;
  set_audio_channels->audio_channels = audio_channels;

  return(set_audio_channels);
}
