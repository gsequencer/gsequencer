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

#include <ags/audio/task/ags_remove_audio.h>

#include <ags/object/ags_connectable.h>

void ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio);
void ags_remove_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_audio_init(AgsRemoveAudio *remove_audio);
void ags_remove_audio_connect(AgsConnectable *connectable);
void ags_remove_audio_disconnect(AgsConnectable *connectable);
void ags_remove_audio_finalize(GObject *gobject);

void ags_remove_audio_launch(AgsTask *task);

/**
 * SECTION:ags_remove_audio
 * @short_description: remove audio object of soundcard
 * @title: AgsRemoveAudio
 * @section_id:
 * @include: ags/audio/task/ags_remove_audio.h
 *
 * The #AgsRemoveAudio task removes #AgsAudio of #AgsSoundcard.
 */

static gpointer ags_remove_audio_parent_class = NULL;
static AgsConnectableInterface *ags_remove_audio_parent_connectable_interface;

GType
ags_remove_audio_get_type()
{
  static GType ags_type_remove_audio = 0;

  if(!ags_type_remove_audio){
    static const GTypeInfo ags_remove_audio_info = {
      sizeof (AgsRemoveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsRemoveAudio\0",
						   &ags_remove_audio_info,
						   0);
    
    g_type_add_interface_static(ags_type_remove_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_audio);
}

void
ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_remove_audio_parent_class = g_type_class_peek_parent(remove_audio);

  /* gobject */
  gobject = (GObjectClass *) remove_audio;

  gobject->finalize = ags_remove_audio_finalize;

  /* task */
  task = (AgsTaskClass *) remove_audio;

  task->launch = ags_remove_audio_launch;
}

void
ags_remove_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_audio_connect;
  connectable->disconnect = ags_remove_audio_disconnect;
}

void
ags_remove_audio_init(AgsRemoveAudio *remove_audio)
{
  remove_audio->soundcard = NULL;
  remove_audio->audio = NULL;
}

void
ags_remove_audio_connect(AgsConnectable *connectable)
{
  ags_remove_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_audio_disconnect(AgsConnectable *connectable)
{
  ags_remove_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_audio_launch(AgsTask *task)
{
  AgsRemoveAudio *remove_audio;
  GList *list;
  
  remove_audio = AGS_REMOVE_AUDIO(task);

  /* remove audio */
  list = ags_soundcard_get_audio(remove_audio->soundcard);
  list = g_list_remove(list,
		       G_OBJECT(remove_audio->audio));

  ags_soundcard_set_audio(remove_audio->soundcard,
			  list)
  g_object_unref(G_OBJECT(remove_audio->audio));
}

/**
 * ags_remove_audio_new:
 * @soundcard: the #GObject implementing #AgsSoundcard
 * @audio: the #AgsAudio to remove
 *
 * Creates an #AgsRemoveAudio.
 *
 * Returns: an new #AgsRemoveAudio.
 *
 * Since: 0.4
 */
AgsRemoveAudio*
ags_remove_audio_new(GObject *soundcard,
		     AgsAudio *audio)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = (AgsRemoveAudio *) g_object_new(AGS_TYPE_REMOVE_AUDIO,
						 NULL);

  remove_audio->soundcard = soundcard;
  remove_audio->audio = audio;

  return(remove_audio);
}
