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

#include <ags/audio/task/ags_seek_soundcard.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#include <ags/i18n.h>

void ags_seek_soundcard_class_init(AgsSeekSoundcardClass *seek_soundcard);
void ags_seek_soundcard_init(AgsSeekSoundcard *seek_soundcard);
void ags_seek_soundcard_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_seek_soundcard_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_seek_soundcard_dispose(GObject *gobject);
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

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_OFFSET,
  PROP_WHENCE,
};

GType
ags_seek_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_seek_soundcard = 0;

    static const GTypeInfo ags_seek_soundcard_info = {
      sizeof(AgsSeekSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_seek_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSeekSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_seek_soundcard_init,
    };


    ags_type_seek_soundcard = g_type_register_static(AGS_TYPE_TASK,
						     "AgsSeekSoundcard",
						     &ags_seek_soundcard_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_seek_soundcard);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_seek_soundcard_dispose;
  gobject->finalize = ags_seek_soundcard_finalize;

  /* properties */
  /**
   * AgsSeekSoundcard:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of seek soundcard"),
				   i18n_pspec("The soundcard of seek soundcard"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);
  
  /**
   * AgsSeekSoundcard:offset:
   *
   * The offset to seek.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_int64("offset",
				 i18n_pspec("offset"),
				 i18n_pspec("The amount of offset"),
				 0,
				 G_MAXINT64,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OFFSET,
				  param_spec);

  /**
   * AgsSeekSoundcard:whence:
   *
   * Whence either AGS_SEEK_SET, AGS_SEEK_CUR or AGS_SEEK_END
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("whence",
				 i18n_pspec("whence"),
				 i18n_pspec("whence"),
				 0,
				 G_MAXUINT,
				 AGS_SEEK_SET,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WHENCE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) seek_soundcard;

  task->launch = ags_seek_soundcard_launch;
}

void
ags_seek_soundcard_init(AgsSeekSoundcard *seek_soundcard)
{
  seek_soundcard->soundcard = NULL;
  seek_soundcard->offset = 0;
  seek_soundcard->whence = AGS_SEEK_SET;
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
  case PROP_OFFSET:
    {
      seek_soundcard->offset = g_value_get_int64(value);
    }
    break;
  case PROP_WHENCE:
    {
      seek_soundcard->whence = g_value_get_uint(value);
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
  case PROP_OFFSET:
    {
      g_value_set_int64(value, seek_soundcard->offset);
    }
    break;
  case PROP_WHENCE:
    {
      g_value_set_uint(value, seek_soundcard->whence);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_seek_soundcard_dispose(GObject *gobject)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = AGS_SEEK_SOUNDCARD(gobject);
  
  if(seek_soundcard->soundcard != NULL){
    g_object_unref(seek_soundcard->soundcard);

    seek_soundcard->soundcard = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_seek_soundcard_parent_class)->dispose(gobject);
}

void
ags_seek_soundcard_finalize(GObject *gobject)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = AGS_SEEK_SOUNDCARD(gobject);
  
  if(seek_soundcard->soundcard != NULL){
    g_object_unref(seek_soundcard->soundcard);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_seek_soundcard_parent_class)->finalize(gobject);
}

void
ags_seek_soundcard_launch(AgsTask *task)
{
  AgsSeekSoundcard *seek_soundcard;

  AgsApplicationContext *application_context;
  
  GObject *soundcard;
  
  GList *audio_start, *audio;
  GList *recall_start, *recall;

  guint note_offset;
  guint note_offset_absolute;

  seek_soundcard = AGS_SEEK_SOUNDCARD(task);

  soundcard = seek_soundcard->soundcard;

  application_context = ags_application_context_get_instance();

  audio = 
    audio_start = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  
  while(audio != NULL){
    /* seek play context */
    g_object_get(audio->data,
		 "play", &recall_start,
		 NULL);

    recall = recall_start;

    while(recall != NULL){
      if(AGS_IS_SEEKABLE(recall->data)){
	ags_seekable_seek(AGS_SEEKABLE(recall->data),
			  seek_soundcard->offset,
			  seek_soundcard->whence);
      }

      recall = recall->next;
    }

    g_list_free_full(recall_start,
		     g_object_unref);

    /* seek recall context */
    g_object_get(audio->data,
		 "recall", &recall_start,
		 NULL);

    recall = recall_start;

    while(recall != NULL){
      if(AGS_IS_SEEKABLE(recall->data)){
	ags_seekable_seek(AGS_SEEKABLE(recall->data),
			  seek_soundcard->offset,
			  seek_soundcard->whence);
      }

      recall = recall->next;
    }

    g_list_free_full(recall_start,
		     g_object_unref);

    /* iterate */
    audio = audio->next;
  }

  g_list_free_full(audio_start,
		   g_object_unref);

  /* seek soundcard */
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard));
  note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard));

  switch(seek_soundcard->whence){
  case AGS_SEEK_CUR:
    {
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    note_offset + seek_soundcard->offset);
      ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					     note_offset_absolute + seek_soundcard->offset);
    }
    break;
  case AGS_SEEK_SET:
    {
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    seek_soundcard->offset);
      ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					     seek_soundcard->offset);
    }
    break;
  case AGS_SEEK_END:
    {
      ags_soundcard_set_note_offset(AGS_SOUNDCARD(seek_soundcard->soundcard),
				    AGS_NOTATION_DEFAULT_END + seek_soundcard->offset);
      ags_soundcard_set_note_offset_absolute(AGS_SOUNDCARD(seek_soundcard->soundcard),
					     AGS_NOTATION_DEFAULT_END + seek_soundcard->offset);
    }
    break;
  }
}

/**
 * ags_seek_soundcard_new:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * @offset: the offset
 * @whence: whence see #AgsSeekType-enum
 *
 * Create a new instance of #AgsSeekSoundcard.
 *
 * Returns: the new #AgsSeekSoundcard
 *
 * Since: 2.0.0
 */
AgsSeekSoundcard*
ags_seek_soundcard_new(GObject *soundcard,
		       gint64 offset,
		       guint whence)
{
  AgsSeekSoundcard *seek_soundcard;

  seek_soundcard = (AgsSeekSoundcard *) g_object_new(AGS_TYPE_SEEK_SOUNDCARD,
						     "soundcard", soundcard,
						     "offset", offset,
						     "whence", whence,
						     NULL);

  return(seek_soundcard);
}
