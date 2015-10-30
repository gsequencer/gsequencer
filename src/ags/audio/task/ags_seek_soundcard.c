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

#include <ags/audio/task/ags_seek_soundcard.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_seekable.h>

#include <ags/audio/ags_audio.h>

void ags_seek_soundcard_class_init(AgsSeekSoundcardClass *seek_soundcard);
void ags_seek_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_seek_soundcard_init(AgsSeekSoundcard *seek_soundcard);
void ags_seek_soundcard_connect(AgsConnectable *connectable);
void ags_seek_soundcard_disconnect(AgsConnectable *connectable);
void ags_seek_soundcard_finalize(GObject *gobject);

void ags_seek_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_seek_soundcard
 * @short_description: seek soundcard object to soundcard loop
 * @title: AgsSeekSoundcard
 * @section_id:
 * @include: ags/soundcard/task/ags_seek_soundcard.h
 *
 * The #AgsSeekSoundcard task seeks #AgsSoundcard to #AgsSoundcardLoop.
 */

static gpointer ags_seek_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_seek_soundcard_parent_connectable_interface;

GType
ags_seek_soundcard_get_type()
{
  static GType ags_type_seek_soundcard = 0;

  if(!ags_type_seek_soundcard){
    static const GTypeInfo ags_seek_soundcard_info = {
      sizeof (AgsSeekSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_seek_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSeekSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_seek_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_seek_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_seek_soundcard = g_type_register_static(AGS_TYPE_TASK,
						     "AgsSeekSoundcard\0",
						     &ags_seek_soundcard_info,
						     0);

    g_type_add_interface_static(ags_type_seek_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_seek_soundcard);
}

void
ags_seek_soundcard_class_init(AgsSeekSoundcardClass *seek_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_seek_soundcard_parent_class = g_type_class_peek_parent(seek_soundcard);

  /* gobject */
  gobject = (GObjectClass *) seek_soundcard;

  gobject->finalize = ags_seek_soundcard_finalize;

  /* task */
  task = (AgsTaskClass *) seek_soundcard;

  task->launch = ags_seek_soundcard_launch;
}

void
ags_seek_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_seek_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_seek_soundcard_connect;
  connectable->disconnect = ags_seek_soundcard_disconnect;
}

void
ags_seek_soundcard_init(AgsSeekSoundcard *seek_soundcard)
{
  seek_soundcard->soundcard = NULL;
  seek_soundcard->steps = 0;
  seek_soundcard->move_forward = FALSE;
}

void
ags_seek_soundcard_connect(AgsConnectable *connectable)
{
  ags_seek_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_seek_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_seek_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_seek_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_seek_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_seek_soundcard_launch(AgsTask *task)
{
  AgsSeekSoundcard *seek_soundcard;

  GList *audio, *recall;

  gdouble delay;
  guint note_offset;

  seek_soundcard = AGS_SEEK_SOUNDCARD(task);

  audio = ags_soundcard_get_audio(AGS_SOUNDCARD(seek_soundcard->soundcard));

  while(audio != NULL){
    recall = AGS_AUDIO(audio->data)->play;

    while(recall != NULL){
      if(AGS_IS_SEEKABLE(recall->data)){
	ags_seekable_seek(AGS_SEEKABLE(recall->data),
			  seek_soundcard->steps,
			  seek_soundcard->move_forward);
      }

      recall = recall->next;
    }

    audio = audio->next;
  }

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(seek_soundcard->soundcard));
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard));
  
  if(seek_soundcard->move_forward){
    ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				  note_offset + (seek_soundcard->steps / delay));
  }else{
    if(note_offset > (seek_soundcard->steps / delay)){
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    note_offset - (seek_soundcard->steps / delay));
    }else{
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    0.0);
    }
  }
}

/**
 * ags_seek_soundcard_new:
 * @soundcard: the #AgsSoundcard to seek
 * @steps:
 * @move_forward:
 *
 * Creates an #AgsSeekSoundcard.
 *
 * Returns: an new #AgsSeekSoundcard.
 *
 * Since: 0.4
 */
AgsSeekSoundcard*
ags_seek_soundcard_new(GObject *soundcard,
		       guint steps,
		       gboolean move_forward)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = (AgsSeekSoundcard *) g_object_new(AGS_TYPE_SEEK_SOUNDCARD,
						     NULL);

  seek_soundcard->soundcard = soundcard;
  seek_soundcard->steps = steps;
  seek_soundcard->move_forward = move_forward;

  return(seek_soundcard);
}
