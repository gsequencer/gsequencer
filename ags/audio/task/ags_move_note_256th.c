/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/task/ags_move_note_256th.h>

#include <ags/i18n.h>

#include <math.h>

static void ags_move_note_256th_class_init(AgsMoveNote256thClass *move_note_256th);
static void ags_move_note_256th_init(AgsMoveNote256th *move_note_256th);
static void ags_move_note_256th_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
static void ags_move_note_256th_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
static void ags_move_note_256th_dispose(GObject *gobject);
static void ags_move_note_256th_finalize(GObject *gobject);

static void ags_move_note_256th_launch(AgsTask *task);

/**
 * SECTION:ags_move_note_256th
 * @short_description: move notation
 * @title: AgsMoveNote256th
 * @section_id:
 * @include: ags/audio/task/ags_move_note_256th.h
 *
 * The #AgsMoveNote256th task moves #AgsNotation.
 */

static gpointer ags_move_note_256th_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_NOTATION,
  PROP_SELECTION,
  PROP_FIRST_X_256th,
  PROP_FIRST_Y,
  PROP_MOVE_X_256th,
  PROP_MOVE_Y,
  PROP_RELATIVE,
  PROP_ABSOLUTE,
};

GType
ags_move_note_256th_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_move_note_256th = 0;

    static const GTypeInfo ags_move_note_256th_info = {
      sizeof(AgsMoveNote256thClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_move_note_256th_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMoveNote256th),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_move_note_256th_init,
    };

    ags_type_move_note_256th = g_type_register_static(AGS_TYPE_TASK,
						      "AgsMoveNote256th",
						      &ags_move_note_256th_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_move_note_256th);
  }

  return(g_define_type_id__static);
}

void
ags_move_note_256th_class_init(AgsMoveNote256thClass *move_note_256th)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_move_note_256th_parent_class = g_type_class_peek_parent(move_note_256th);

  /* gobject */
  gobject = (GObjectClass *) move_note_256th;

  gobject->set_property = ags_move_note_256th_set_property;
  gobject->get_property = ags_move_note_256th_get_property;

  gobject->dispose = ags_move_note_256th_dispose;
  gobject->finalize = ags_move_note_256th_finalize;

  /* properties */
  /**
   * AgsMoveNote256th:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 9.1.3
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
   * AgsMoveNote256th:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 9.1.3
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
   * AgsMoveNote256th:selection: (type GList(AgsNote))
   *
   * The assigned #AgsNote
   * 
   * Since: 9.1.3
   */
  param_spec = g_param_spec_pointer("selection",
				    i18n_pspec("selection to move"),
				    i18n_pspec("The selection to move"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION,
				  param_spec);

  /**
   * AgsMoveNote256th:first-x_256th:
   *
   * Move notation from x offset.
   * 
   * Since: 9.1.3
   */
  param_spec =  g_param_spec_uint("first-x-256th",
				  i18n_pspec("move from x 256th offset"),
				  i18n_pspec("Move the notation from x 256th offset"),
				  0,
				  AGS_MOVE_NOTE_256TH_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_X_256th,
				  param_spec);

  /**
   * AgsMoveNote256th:first-y:
   *
   * Move notation from first y.
   * 
   * Since: 9.1.3
   */
  param_spec =  g_param_spec_uint("first-y",
				  i18n_pspec("move from y offset"),
				  i18n_pspec("Move the notation from y offset"),
				  0,
				  AGS_MOVE_NOTE_256TH_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_Y,
				  param_spec);

  /**
   * AgsMoveNote256th:move-x-256th:
   *
   * Move notation by move x 256th amount.
   * 
   * Since: 9.1.3
   */
  param_spec = g_param_spec_int("move-x-256th",
				i18n_pspec("move with move x 256th amount"),
				i18n_pspec("Move the notation by move x 256th amount"),
				-1 * AGS_MOVE_NOTE_256TH_DEFAULT_X_LENGTH,
				AGS_MOVE_NOTE_256TH_DEFAULT_X_LENGTH,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MOVE_X_256th,
				  param_spec);
  
  /**
   * AgsMoveNote256th:move-y:
   *
   * Move notation by move y amount.
   * 
   * Since: 9.1.3
   */
  param_spec =  g_param_spec_int("move-y",
				 i18n_pspec("move with move y amount"),
				 i18n_pspec("Move the notation by move y amount"),
				 -1 * AGS_MOVE_NOTE_256TH_DEFAULT_Y_LENGTH,
				 AGS_MOVE_NOTE_256TH_DEFAULT_Y_LENGTH,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MOVE_Y,
				  param_spec);

  /**
   * AgsMoveNote256th:relative:
   *
   * Move notation by relative position.
   * 
   * Since: 9.1.3
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
   * AgsMoveNote256th:absolute:
   *
   * Move notation by absolute position.
   * 
   * Since: 9.1.3
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
  task = (AgsTaskClass *) move_note_256th;

  task->launch = ags_move_note_256th_launch;
}

void
ags_move_note_256th_init(AgsMoveNote256th *move_note_256th)
{
  move_note_256th->audio = NULL;
  move_note_256th->notation = NULL;

  move_note_256th->selection = NULL;
  
  move_note_256th->first_x_256th = 0;
  move_note_256th->first_y = 0;
  move_note_256th->move_x_256th = 0;
  move_note_256th->move_y = 0;

  move_note_256th->relative = FALSE;
  move_note_256th->absolute = FALSE;
}

void
ags_move_note_256th_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsMoveNote256th *move_note_256th;

  move_note_256th = AGS_MOVE_NOTE_256TH(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(move_note_256th->audio == audio){
	return;
      }

      if(move_note_256th->audio != NULL){
	g_object_unref(move_note_256th->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      move_note_256th->audio = audio;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(move_note_256th->notation == notation){
	return;
      }

      if(move_note_256th->notation != NULL){
	g_object_unref(move_note_256th->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      move_note_256th->notation = notation;
    }
    break;
  case PROP_SELECTION:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_pointer(value);

      if(note == NULL ||
	 g_list_find(move_note_256th->selection, note) != NULL){
	return;
      }

      g_object_ref(note);
      move_note_256th->selection = g_list_prepend(move_note_256th->selection,
						  note);
    }
    break;
  case PROP_FIRST_X_256th:
    {
      move_note_256th->first_x_256th = g_value_get_uint(value);
    }
    break;
  case PROP_FIRST_Y:
    {
      move_note_256th->first_y = g_value_get_uint(value);
    }
    break;
  case PROP_MOVE_X_256th:
    {
      move_note_256th->move_x_256th = g_value_get_int(value);
    }
    break;
  case PROP_MOVE_Y:
    {
      move_note_256th->move_y = g_value_get_int(value);
    }
    break;
  case PROP_RELATIVE:
    {
      move_note_256th->relative = g_value_get_boolean(value);
    }
    break;
  case PROP_ABSOLUTE:
    {
      move_note_256th->absolute = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_256th_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsMoveNote256th *move_note_256th;

  move_note_256th = AGS_MOVE_NOTE_256TH(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, move_note_256th->audio);
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, move_note_256th->notation);
    }
    break;
  case PROP_SELECTION:
    {
      g_value_set_pointer(value,
			  g_list_copy_deep(move_note_256th->selection,
					   (GCopyFunc) g_object_ref,
					   NULL));
    }
    break;
  case PROP_FIRST_X_256th:
    {
      g_value_set_int(value, move_note_256th->first_x_256th);
    }
    break;
  case PROP_FIRST_Y:
    {
      g_value_set_uint(value, move_note_256th->first_y);
    }
    break;
  case PROP_MOVE_X_256th:
    {
      g_value_set_uint(value, move_note_256th->move_x_256th);
    }
    break;
  case PROP_MOVE_Y:
    {
      g_value_set_uint(value, move_note_256th->move_y);
    }
    break;
  case PROP_RELATIVE:
    {
      g_value_set_boolean(value, move_note_256th->relative);
    }
    break;
  case PROP_ABSOLUTE:
    {
      g_value_set_boolean(value, move_note_256th->absolute);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_256th_dispose(GObject *gobject)
{
  AgsMoveNote256th *move_note_256th;

  move_note_256th = AGS_MOVE_NOTE_256TH(gobject);

  if(move_note_256th->audio != NULL){
    g_object_unref(move_note_256th->audio);

    move_note_256th->audio = NULL;
  }

  if(move_note_256th->notation != NULL){
    g_object_unref(move_note_256th->notation);

    move_note_256th->notation = NULL;
  }

  if(move_note_256th->selection != NULL){
    g_list_free(move_note_256th->selection);

    move_note_256th->selection = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_move_note_256th_parent_class)->dispose(gobject);
}

void
ags_move_note_256th_finalize(GObject *gobject)
{
  AgsMoveNote256th *move_note_256th;

  move_note_256th = AGS_MOVE_NOTE_256TH(gobject);

  if(move_note_256th->audio != NULL){
    g_object_unref(move_note_256th->audio);
  }

  if(move_note_256th->notation != NULL){
    g_object_unref(move_note_256th->notation);
  }

  if(move_note_256th->selection != NULL){
    g_list_free(move_note_256th->selection);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_move_note_256th_parent_class)->finalize(gobject);
}

void
ags_move_note_256th_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsNotation *notation, *current_notation;
  AgsNote *note;
  
  AgsMoveNote256th *move_note_256th;

  GList *selection;

  guint audio_channel;
  guint first_x_256th;
  guint first_y;
  gint move_x_256th;
  gint move_y;
  
  gboolean relative;
  gboolean absolute;

  move_note_256th = AGS_MOVE_NOTE_256TH(task);

  g_return_if_fail(AGS_IS_AUDIO(move_note_256th->audio));
  g_return_if_fail(AGS_IS_NOTATION(move_note_256th->notation));

  g_object_get(move_note_256th,
	       "audio", &audio,
	       NULL);
  
  /* get some properties */
  notation =
    current_notation = move_note_256th->notation;

  g_object_get(notation,
	       "audio-channel", &audio_channel,
	       NULL);

  selection = move_note_256th->selection;

  first_x_256th = move_note_256th->first_x_256th;
  first_y = move_note_256th->first_y;

  move_x_256th = move_note_256th->move_x_256th;
  move_y = move_note_256th->move_y;
  
  relative = move_note_256th->relative;
  absolute = move_note_256th->absolute;

  /* move */
  while(selection != NULL){
    note = ags_note_duplicate(AGS_NOTE(selection->data));

    if(relative){
      note->x[0] = note->x[0] + (move_x_256th / 16);
      note->x[1] = note->x[1] + (move_x_256th / 16);

      note->x_256th[0] = note->x_256th[0] + move_x_256th;
      note->x_256th[1] = note->x_256th[1] + move_x_256th;
      
      note->y = note->y + move_y;
    }else if(absolute){
      note->x[0] = (move_x_256th / 16) + (note->x[0] - (first_x_256th / 16));
      note->x[1] = (move_x_256th / 16) + (note->x[1] - (first_x_256th / 16));

      note->x_256th[0] = move_x_256th + (note->x_256th[0] - (first_x_256th));
      note->x_256th[1] = move_x_256th + (note->x_256th[1] - (first_x_256th));

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
 * ags_move_note_256th_set_selection:
 * @move_note_256th: the #AgsMoveNote256th
 * @selection: (element-type AgsAudio.Note256th) (transfer none): the selection as #GList-struct
 * 
 * Set @selection of @move_note_256th.
 * 
 * Since: 9.1.3
 */
void
ags_move_note_256th_set_selection(AgsMoveNote256th *move_note_256th,
				  GList *selection)
{
  if(!AGS_IS_MOVE_NOTE_256TH(move_note_256th) ||
     move_note_256th->selection == selection){
    return;
  }

  if(move_note_256th->selection != NULL){
    g_list_free_full(move_note_256th->selection,
		     (GDestroyNotify) g_object_unref);
  }
  
  move_note_256th->selection = g_list_copy_deep(selection,
						(GCopyFunc) g_object_ref,
						NULL);
}

/**
 * ags_move_note_256th_new:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 * @selection: (element-type AgsAudio.Note256th) (transfer none): the selection as #GList-struct
 * @first_x: the x offset to move from 
 * @first_y: the x padding to use
 * @move_x: the amout to move in x direction
 * @move_y: the amout to move in y direction
 * @relative: if %TRUE move relative position
 * @absolute: if %TRUE move absolute position
 *
 * Create a new instance of #AgsMoveNote256th task. Note256th either @relative or @absolute shall
 * be %TRUE else it won't have any effect.
 *
 * Returns: a new #AgsMoveNote256th
 *
 * Since: 9.1.3
 */
AgsMoveNote256th*
ags_move_note_256th_new(AgsAudio *audio,
			AgsNotation *notation,
			GList *selection,
			guint first_x, guint first_y,
			gint move_x, gint move_y,
			gboolean relative, gboolean absolute)
{
  AgsMoveNote256th *move_note_256th;
  
  move_note_256th = (AgsMoveNote256th *) g_object_new(AGS_TYPE_MOVE_NOTE_256TH,
						      "audio", audio,
						      "notation", notation,
						      "first-x", first_x,
						      "first-y", first_y,
						      "move-x", move_x,
						      "move-y", move_y,
						      "relative", relative,
						      "absolute", absolute,
						      NULL);

  ags_move_note_256th_set_selection(move_note_256th,
				    selection);

  return(move_note_256th);
}
