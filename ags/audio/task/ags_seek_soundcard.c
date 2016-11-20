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
void ags_seek_soundcard_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_seek_soundcard_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_seek_soundcard_connect(AgsConnectable *connectable);
void ags_seek_soundcard_disconnect(AgsConnectable *connectable);
void ags_seek_soundcard_finalize(GObject *gobject);

void ags_seek_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_seek_soundcard
 * @short_description: seek soundcard object
 * @title: AgsSeekSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_seek_soundcard.h
 *
 * The #AgsSeekSoundcard task seeks #AgsSoundcard.
 */

static gpointer ags_seek_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_seek_soundcard_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_STEPS,
  PROP_MOVE_FORWARD,
};

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
  GParamSpec *param_spec;

  ags_seek_soundcard_parent_class = g_type_class_peek_parent(seek_soundcard);

  /* gobject */
  gobject = (GObjectClass *) seek_soundcard;

  gobject->set_property = ags_seek_soundcard_set_property;
  gobject->get_property = ags_seek_soundcard_get_property;

  gobject->finalize = ags_seek_soundcard_finalize;

  /* properties */
  /**
   * AgsSeekSoundcard:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of seek soundcard\0",
				   "The soundcard of seek soundcard\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);
  
  /**
   * AgsSeekSoundcard:steps:
   *
   * The amount of steps to seek.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("steps\0",
				 "steps\0",
				 "The amount of steps\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEPS,
				  param_spec);

  /**
   * AgsSeekSoundcard:move-forward:
   *
   * The notation's move-forward.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_boolean("move-forward\0",
				    "move forward\0",
				    "Do moving forward\0",
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MOVE_FORWARD,
				  param_spec);

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
ags_seek_soundcard_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = AGS_SEEK_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(seek_soundcard->soundcard == (GObject *) soundcard){
	return;
      }

      if(seek_soundcard->soundcard != NULL){
	g_object_unref(seek_soundcard->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      seek_soundcard->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_STEPS:
    {
      seek_soundcard->steps = g_value_get_uint(value);
    }
    break;
  case PROP_MOVE_FORWARD:
    {
      seek_soundcard->move_forward = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_seek_soundcard_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = AGS_SEEK_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, seek_soundcard->soundcard);
    }
    break;
  case PROP_STEPS:
    {
      g_value_set_uint(value, seek_soundcard->steps);
    }
    break;
  case PROP_MOVE_FORWARD:
    {
      g_value_set_boolean(value, seek_soundcard->move_forward);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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

  guint note_offset;
  guint note_offset_absolute;

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
  
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard));
  note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard));
  
  if(seek_soundcard->move_forward){
    ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				  note_offset + seek_soundcard->steps);

    ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					   note_offset_absolute + seek_soundcard->steps);
  }else{
    if(note_offset > seek_soundcard->steps){
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    note_offset - seek_soundcard->steps);

      ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					     note_offset_absolute - seek_soundcard->steps);
    }else{
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    0.0);

      ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					     0.0);
    }
  }
}

/**
 * ags_seek_soundcard_new:
 * @soundcard: the #AgsSoundcard to seek
 * @steps: steps
 * @move_forward: moves forward if %TRUE, otherwise backward
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
