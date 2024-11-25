/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/task/ags_move_note.h>

#include <ags/i18n.h>

#include <math.h>

void ags_move_note_class_init(AgsMoveNoteClass *move_note);
void ags_move_note_init(AgsMoveNote *move_note);
void ags_move_note_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_move_note_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_move_note_dispose(GObject *gobject);
void ags_move_note_finalize(GObject *gobject);

void ags_move_note_launch(AgsTask *task);

/**
 * SECTION:ags_move_note
 * @short_description: move notation
 * @title: AgsMoveNote
 * @section_id:
 * @include: ags/audio/task/ags_move_note.h
 *
 * The #AgsMoveNote task moves #AgsNotation.
 */

static gpointer ags_move_note_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_NOTATION,
  PROP_SELECTION,
  PROP_FIRST_X,
  PROP_FIRST_Y,
  PROP_MOVE_X,
  PROP_MOVE_Y,
  PROP_RELATIVE,
  PROP_ABSOLUTE,
};

GType
ags_move_note_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_move_note = 0;

    static const GTypeInfo ags_move_note_info = {
      sizeof(AgsMoveNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_move_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMoveNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_move_note_init,
    };

    ags_type_move_note = g_type_register_static(AGS_TYPE_TASK,
						"AgsMoveNote",
						&ags_move_note_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_move_note);
  }

  return g_define_type_id__volatile;
}

void
ags_move_note_class_init(AgsMoveNoteClass *move_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_move_note_parent_class = g_type_class_peek_parent(move_note);

  /* gobject */
  gobject = (GObjectClass *) move_note;

  gobject->set_property = ags_move_note_set_property;
  gobject->get_property = ags_move_note_get_property;

  gobject->dispose = ags_move_note_dispose;
  gobject->finalize = ags_move_note_finalize;

  /* properties */
  /**
   * AgsMoveNote:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of move note"),
				   i18n_pspec("The audio of move note task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsMoveNote:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("notation of move note"),
				   i18n_pspec("The notation of move note task"),
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
  
  /**
   * AgsMoveNote:selection: (type GList(AgsNote))
   *
   * The assigned #AgsNote
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("selection",
				    i18n_pspec("selection to move"),
				    i18n_pspec("The selection to move"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION,
				  param_spec);

  /**
   * AgsMoveNote:first-x:
   *
   * Move notation from x offset.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("first-x",
				  i18n_pspec("move from x offset"),
				  i18n_pspec("Move the notation from x offset"),
				  0,
				  AGS_MOVE_NOTE_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_X,
				  param_spec);

  /**
   * AgsMoveNote:first-y:
   *
   * Move notation with x padding.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("first-y",
				  i18n_pspec("move with x padding"),
				  i18n_pspec("Move the notation with x padding"),
				  0,
				  AGS_MOVE_NOTE_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_Y,
				  param_spec);

  /**
   * AgsMoveNote:move-x:
   *
   * Move notation by move-x amount.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("move-x",
				i18n_pspec("move with move-x amount"),
				i18n_pspec("Move the notation by move-x amount"),
				-1 * AGS_MOVE_NOTE_DEFAULT_X_LENGTH,
				AGS_MOVE_NOTE_DEFAULT_X_LENGTH,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MOVE_X,
				  param_spec);
  
  /**
   * AgsMoveNote:move-y:
   *
   * Move notation by move-y amount.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("move-y",
				 i18n_pspec("move with move-y amount"),
				 i18n_pspec("Move the notation by move-y amount"),
				 -1 * AGS_MOVE_NOTE_DEFAULT_Y_LENGTH,
				 AGS_MOVE_NOTE_DEFAULT_Y_LENGTH,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MOVE_Y,
				  param_spec);

  /**
   * AgsMoveNote:relative:
   *
   * Move notation by relative position.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("relative",
				     i18n_pspec("move relative"),
				     i18n_pspec("Move the notation by relative position"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RELATIVE,
				  param_spec);

  /**
   * AgsMoveNote:absolute:
   *
   * Move notation by absolute position.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("absolute",
				     i18n_pspec("move absolute"),
				     i18n_pspec("Move the notation by absolute position"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ABSOLUTE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) move_note;

  task->launch = ags_move_note_launch;
}

void
ags_move_note_init(AgsMoveNote *move_note)
{
  move_note->audio = NULL;
  move_note->notation = NULL;

  move_note->selection = NULL;
  
  move_note->first_x = 0;
  move_note->first_y = 0;
  move_note->move_x = 0;
  move_note->move_y = 0;

  move_note->relative = FALSE;
  move_note->absolute = FALSE;
}

void
ags_move_note_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsMoveNote *move_note;

  move_note = AGS_MOVE_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(move_note->audio == audio){
	return;
      }

      if(move_note->audio != NULL){
	g_object_unref(move_note->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      move_note->audio = audio;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(move_note->notation == notation){
	return;
      }

      if(move_note->notation != NULL){
	g_object_unref(move_note->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      move_note->notation = notation;
    }
    break;
  case PROP_SELECTION:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_pointer(value);

      if(note == NULL ||
	 g_list_find(move_note->selection, note) != NULL){
	return;
      }

      g_object_ref(note);
      move_note->selection = g_list_prepend(move_note->selection,
					    note);
    }
    break;
  case PROP_FIRST_X:
    {
      move_note->first_x = g_value_get_uint(value);
    }
    break;
  case PROP_FIRST_Y:
    {
      move_note->first_y = g_value_get_uint(value);
    }
    break;
  case PROP_MOVE_X:
    {
      move_note->move_x = g_value_get_int(value);
    }
    break;
  case PROP_MOVE_Y:
    {
      move_note->move_y = g_value_get_int(value);
    }
    break;
  case PROP_RELATIVE:
    {
      move_note->relative = g_value_get_boolean(value);
    }
    break;
  case PROP_ABSOLUTE:
    {
      move_note->absolute = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsMoveNote *move_note;

  move_note = AGS_MOVE_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, move_note->audio);
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, move_note->notation);
    }
    break;
  case PROP_SELECTION:
    {
      g_value_set_pointer(value,
			  g_list_copy_deep(move_note->selection,
					   (GCopyFunc) g_object_ref,
					   NULL));
    }
    break;
  case PROP_FIRST_X:
    {
      g_value_set_int(value, move_note->first_x);
    }
    break;
  case PROP_FIRST_Y:
    {
      g_value_set_uint(value, move_note->first_y);
    }
    break;
  case PROP_MOVE_X:
    {
      g_value_set_uint(value, move_note->move_x);
    }
    break;
  case PROP_MOVE_Y:
    {
      g_value_set_uint(value, move_note->move_y);
    }
    break;
  case PROP_RELATIVE:
    {
      g_value_set_boolean(value, move_note->relative);
    }
    break;
  case PROP_ABSOLUTE:
    {
      g_value_set_boolean(value, move_note->absolute);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_dispose(GObject *gobject)
{
  AgsMoveNote *move_note;

  move_note = AGS_MOVE_NOTE(gobject);

  if(move_note->audio != NULL){
    g_object_unref(move_note->audio);

    move_note->audio = NULL;
  }

  if(move_note->notation != NULL){
    g_object_unref(move_note->notation);

    move_note->notation = NULL;
  }

  if(move_note->selection != NULL){
    g_list_free(move_note->selection);

    move_note->selection = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_move_note_parent_class)->dispose(gobject);
}

void
ags_move_note_finalize(GObject *gobject)
{
  AgsMoveNote *move_note;

  move_note = AGS_MOVE_NOTE(gobject);

  if(move_note->audio != NULL){
    g_object_unref(move_note->audio);
  }

  if(move_note->notation != NULL){
    g_object_unref(move_note->notation);
  }

  if(move_note->selection != NULL){
    g_list_free(move_note->selection);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_move_note_parent_class)->finalize(gobject);
}

void
ags_move_note_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsNotation *notation, *current_notation;
  AgsNote *note;
  
  AgsMoveNote *move_note;

  GList *selection;

  guint audio_channel;
  guint first_x;
  guint first_y;
  gint move_x;
  gint move_y;
  
  gboolean relative;
  gboolean absolute;

  move_note = AGS_MOVE_NOTE(task);

  g_return_if_fail(AGS_IS_AUDIO(move_note->audio));
  g_return_if_fail(AGS_IS_NOTATION(move_note->notation));

  g_object_get(move_note,
	       "audio", &audio,
	       NULL);
  
  /* get some properties */
  notation =
    current_notation = move_note->notation;

  g_object_get(notation,
	       "audio-channel", &audio_channel,
	       NULL);

  selection = move_note->selection;

  first_x = move_note->first_x;
  first_y = move_note->first_y;

  move_x = move_note->move_x;
  move_y = move_note->move_y;
  
  relative = move_note->relative;
  absolute = move_note->absolute;

  /* move */
  while(selection != NULL){
    note = ags_note_duplicate(AGS_NOTE(selection->data));

    if(relative){
      note->x[0] = note->x[0] + move_x;
      note->x[1] = note->x[1] + move_x;

      note->x_256th[0] = note->x_256th[0] + (16 * move_x);
      note->x_256th[1] = note->x_256th[1] + (16 * move_x);
      
      note->y = note->y + move_y;
    }else if(absolute){
      note->x[0] = move_x + (note->x[0] - first_x);
      note->x[1] = move_x + (note->x[1] - first_x);

      note->x_256th[0] = move_x + (note->x_256th[0] - (16 * first_x));
      note->x_256th[1] = (16 * move_x) + (note->x_256th[1] - (16 * first_x));

      note->y = move_y + (note->y + first_y);
    }

    if(note->x[0] >= ags_timestamp_get_ags_offset(current_notation->timestamp) + AGS_NOTATION_DEFAULT_OFFSET){
      AgsTimestamp *timestamp;

      GList *list_start, *list;

      g_object_get(audio,
		   "notation", &list_start,
		   NULL);
      
      timestamp = ags_timestamp_new();
      timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
      timestamp->flags |= AGS_TIMESTAMP_OFFSET;
      
      timestamp->timer.ags_offset.offset = (guint64) (AGS_NOTATION_DEFAULT_OFFSET * floor(note->x[0] / AGS_NOTATION_DEFAULT_OFFSET));

      if((list = ags_notation_find_near_timestamp(list_start, audio_channel,
						  timestamp)) == NULL){
	current_notation = ags_notation_new((GObject *) audio,
					    audio_channel);
	
	current_notation->timestamp->timer.ags_offset.offset = ags_timestamp_get_ags_offset(timestamp);
	ags_audio_add_notation(audio,
			       (GObject *) current_notation);
      }else{
	current_notation = list->data;
      }

      g_list_free_full(list_start,
		       g_object_unref);
      g_object_unref(timestamp);
    }

    /* remove old note */
    ags_notation_remove_note(notation,
			     selection->data,
			     TRUE);
    ags_notation_remove_note(notation,
			     selection->data,
			     FALSE);

    /* add new note */
    ags_notation_add_note(current_notation,
			  note,
			  FALSE);

    selection = selection->next;
  }

  g_object_unref(audio);
}

/**
 * ags_move_note_set_selection:
 * @move_note: the #AgsMoveNote
 * @selection: (element-type AgsAudio.Note) (transfer none): the selection as #GList-struct
 * 
 * Set @selection of @move_note.
 * 
 * Since: 3.2.3
 */
void
ags_move_note_set_selection(AgsMoveNote *move_note,
			    GList *selection)
{
  if(!AGS_IS_MOVE_NOTE(move_note) ||
     move_note->selection == selection){
    return;
  }

  if(move_note->selection != NULL){
    g_list_free_full(move_note->selection,
		     (GDestroyNotify) g_object_unref);
  }
  
  move_note->selection = g_list_copy_deep(selection,
					  (GCopyFunc) g_object_ref,
					  NULL);
}

/**
 * ags_move_note_new:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 * @selection: (element-type AgsAudio.Note) (transfer none): the selection as #GList-struct
 * @first_x: the x offset to move from 
 * @first_y: the x padding to use
 * @move_x: the amout to move in x direction
 * @move_y: the amout to move in y direction
 * @relative: if %TRUE move relative position
 * @absolute: if %TRUE move absolute position
 *
 * Create a new instance of #AgsMoveNote task. Note either @relative or @absolute shall
 * be %TRUE else it won't have any effect.
 *
 * Returns: a new #AgsMoveNote
 *
 * Since: 3.0.0
 */
AgsMoveNote*
ags_move_note_new(AgsAudio *audio,
		  AgsNotation *notation,
		  GList *selection,
		  guint first_x, guint first_y,
		  gint move_x, gint move_y,
		  gboolean relative, gboolean absolute)
{
  AgsMoveNote *move_note;
  
  move_note = (AgsMoveNote *) g_object_new(AGS_TYPE_MOVE_NOTE,
					   "audio", audio,
					   "notation", notation,
					   "first-x", first_x,
					   "first-y", first_y,
					   "move-x", move_x,
					   "move-y", move_y,
					   "relative", relative,
					   "absolute", absolute,
					   NULL);

  ags_move_note_set_selection(move_note,
			      selection);

  return(move_note);
}
