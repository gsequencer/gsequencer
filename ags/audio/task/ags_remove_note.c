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

#include <ags/audio/task/ags_remove_note.h>

#include <ags/audio/ags_notation.h>

#include <ags/i18n.h>

void ags_remove_note_class_init(AgsRemoveNoteClass *remove_note);
void ags_remove_note_init(AgsRemoveNote *remove_note);
void ags_remove_note_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_remove_note_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_remove_note_dispose(GObject *gobject);
void ags_remove_note_finalize(GObject *gobject);

void ags_remove_note_launch(AgsTask *task);

/**
 * SECTION:ags_remove_note
 * @short_description: remove note object from notation
 * @title: AgsRemoveNote
 * @section_id:
 * @include: ags/audio/task/ags_remove_note.h
 *
 * The #AgsRemoveNote task removes #AgsNote from #AgsNotation.
 */

static gpointer ags_remove_note_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_NOTE,
  PROP_AUDIO_CHANNEL,
  PROP_USE_SELECTION_LIST,
};

GType
ags_remove_note_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_remove_note = 0;

    static const GTypeInfo ags_remove_note_info = {
      sizeof(AgsRemoveNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRemoveNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_note_init,
    };

    ags_type_remove_note = g_type_register_static(AGS_TYPE_TASK,
						  "AgsRemoveNote",
						  &ags_remove_note_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_remove_note);
  }

  return g_define_type_id__volatile;
}

void
ags_remove_note_class_init(AgsRemoveNoteClass *remove_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_remove_note_parent_class = g_type_class_peek_parent(remove_note);

  /* gobject */
  gobject = (GObjectClass *) remove_note;

  gobject->set_property = ags_remove_note_set_property;
  gobject->get_property = ags_remove_note_get_property;

  gobject->dispose = ags_remove_note_dispose;
  gobject->finalize = ags_remove_note_finalize;

  /* properties */
  /**
   * AgsRemoveNote:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of remove note"),
				   i18n_pspec("The audio of remove note task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);
  
  /**
   * AgsRemoveNote:note:
   *
   * The assigned #AgsNote
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("note",
				   i18n_pspec("note of remove note"),
				   i18n_pspec("The note of remove note task"),
				   AGS_TYPE_NOTE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /**
   * AgsRemoveNote:audio-channel:
   *
   * The assigned audio channel
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("audio-channel",
				 i18n_pspec("audio channel of notation"),
				 i18n_pspec("The audio channel of notation"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /**
   * AgsRemoveNote:use-selection-list:
   *
   * The notation's use-selection-list.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_boolean("use-selection-list",
				     i18n_pspec("use selection list"),
				     i18n_pspec("Use selection list of notation"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_USE_SELECTION_LIST,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) remove_note;

  task->launch = ags_remove_note_launch;
}

void
ags_remove_note_init(AgsRemoveNote *remove_note)
{
  remove_note->audio = NULL;
  remove_note->note = NULL;

  remove_note->audio_channel = 0;
  remove_note->use_selection_list = FALSE;
}

void
ags_remove_note_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(remove_note->audio == audio){
	return;
      }

      if(remove_note->audio != NULL){
	g_object_unref(remove_note->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      remove_note->audio = audio;
    }
    break;
  case PROP_NOTE:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_object(value);

      if(remove_note->note == note){
	return;
      }

      if(remove_note->note != NULL){
	g_object_unref(remove_note->note);
      }

      if(note != NULL){
	g_object_ref(note);
      }

      remove_note->note = note;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      remove_note->audio_channel = g_value_get_uint(value);
    }
  break;
  case PROP_USE_SELECTION_LIST:
    {
      remove_note->use_selection_list = g_value_get_boolean(value);
    }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_note_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, remove_note->audio);
    }
    break;
  case PROP_NOTE:
    {
      g_value_set_object(value, remove_note->note);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, remove_note->audio_channel);
    }
    break;
  case PROP_USE_SELECTION_LIST:
    {
      g_value_set_boolean(value, remove_note->use_selection_list);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_note_dispose(GObject *gobject)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  if(remove_note->audio != NULL){
    g_object_unref(remove_note->audio);

    remove_note->audio = NULL;
  }

  if(remove_note->note != NULL){
    g_object_unref(remove_note->note);

    remove_note->note = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_remove_note_parent_class)->dispose(gobject);
}

void
ags_remove_note_finalize(GObject *gobject)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  if(remove_note->audio != NULL){
    g_object_unref(remove_note->audio);
  }

  if(remove_note->note != NULL){
    g_object_unref(remove_note->note);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_remove_note_parent_class)->finalize(gobject);
}

void
ags_remove_note_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;
  
  AgsRemoveNote *remove_note;

  AgsTimestamp *timestamp;

  GList *list;

  guint audio_channel;
  guint x0;
  
  pthread_mutex_t *audio_mutex;

  remove_note = AGS_REMOVE_NOTE(task);

  /* get some fields */
  audio = remove_note->audio;

  notation = NULL;
  audio_channel = remove_note->audio_channel;
  
  note = remove_note->note;

  g_object_get(note,
	       "x0", &x0,
	       NULL);

  /* create timestamp */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(x0 / AGS_NOTATION_DEFAULT_OFFSET);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* find near timestamp */
  pthread_mutex_lock(audio_mutex);

  list = ags_notation_find_near_timestamp(audio->notation, audio_channel,
					  timestamp);

  pthread_mutex_unlock(audio_mutex);
					  
  if(list == NULL){
    return;
  }

  /* remove note */
  notation = list->data;
  
  ags_notation_remove_note(notation,
			   note,
			   remove_note->use_selection_list);
  
  g_object_unref(timestamp);
}

/**
 * ags_remove_note_new:
 * @audio: the #AgsAudio
 * @note: the #AgsNote to remove
 * @use_selection_list: if %TRUE remove of selection, otherwise of notation
 *
 * Create a new instance of #AgsRemoveNote.
 *
 * Returns: the new #AgsRemoveNote
 *
 * Since: 2.0.0
 */
AgsRemoveNote*
ags_remove_note_new(AgsAudio *audio,
		    AgsNote *note,
		    guint audio_channel,
		    gboolean use_selection_list)
{
  AgsRemoveNote *remove_note;

  remove_note = (AgsRemoveNote *) g_object_new(AGS_TYPE_REMOVE_NOTE,
					       "audio", audio,
					       "note", note,
					       "audio-channel", audio_channel, 
					       "use-selection-list", use_selection_list,
					       NULL);

  return(remove_note);
}
