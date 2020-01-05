/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/task/ags_crop_note.h>

#include <ags/i18n.h>

#include <math.h>

void ags_crop_note_class_init(AgsCropNoteClass *crop_note);
void ags_crop_note_init(AgsCropNote *crop_note);
void ags_crop_note_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_crop_note_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_crop_note_dispose(GObject *gobject);
void ags_crop_note_finalize(GObject *gobject);

void ags_crop_note_launch(AgsTask *task);

/**
 * SECTION:ags_crop_note
 * @short_description: crop notation
 * @title: AgsCropNote
 * @section_id:
 * @include: ags/audio/task/ags_crop_note.h
 *
 * The #AgsCropNote task crops #AgsNotation.
 */

static gpointer ags_crop_note_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_NOTATION,
  PROP_SELECTION,
  PROP_X_PADDING,
  PROP_X_CROP,
  PROP_ABSOLUTE,
  PROP_IN_PLACE,
  PROP_DO_RESIZE,
};

GType
ags_crop_note_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_crop_note = 0;

    static const GTypeInfo ags_crop_note_info = {
      sizeof(AgsCropNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCropNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_init,
    };

    ags_type_crop_note = g_type_register_static(AGS_TYPE_TASK,
						"AgsCropNote",
						&ags_crop_note_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_crop_note);
  }

  return g_define_type_id__volatile;
}

void
ags_crop_note_class_init(AgsCropNoteClass *crop_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_crop_note_parent_class = g_type_class_peek_parent(crop_note);

  /* gobject */
  gobject = (GObjectClass *) crop_note;

  gobject->set_property = ags_crop_note_set_property;
  gobject->get_property = ags_crop_note_get_property;

  gobject->dispose = ags_crop_note_dispose;
  gobject->finalize = ags_crop_note_finalize;

  /* properties */
  /**
   * AgsCropNote:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of crop note"),
				   i18n_pspec("The audio of crop note task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsCropNote:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("notation of crop note"),
				   i18n_pspec("The notation of crop note task"),
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
  
  /**
   * AgsCropNote:selection:
   *
   * The assigned #AgsNote
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("selection",
				    i18n_pspec("selection to crop"),
				    i18n_pspec("The selection to crop"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION,
				  param_spec);

  /**
   * AgsCropNote:x-padding:
   *
   * Crop notation with x padding.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("x-padding",
				  i18n_pspec("crop with x padding"),
				  i18n_pspec("Crop the notation with x padding"),
				  0,
				  AGS_CROP_NOTE_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_PADDING,
				  param_spec);

  /**
   * AgsCropNote:x-crop:
   *
   * Crop notation by x-crop amount.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("x-crop",
				 i18n_pspec("crop with x-crop amount"),
				 i18n_pspec("Crop the notation by x-crop amount"),
				 -1 * AGS_CROP_NOTE_DEFAULT_X_LENGTH,
				 AGS_CROP_NOTE_DEFAULT_X_LENGTH,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_CROP,
				  param_spec);

  /**
   * AgsCropNote:absolute:
   *
   * Crop notation by absolute position.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("absolute",
				     i18n_pspec("crop absolute"),
				     i18n_pspec("Crop the notation by absolute position"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ABSOLUTE,
				  param_spec);

  /**
   * AgsCropNote:in-place:
   *
   * Crop notation in place.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("in-place",
				     i18n_pspec("crop in place"),
				     i18n_pspec("Crop the notation by in place"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IN_PLACE,
				  param_spec);

  /**
   * AgsCropNote:do-resize:
   *
   * Crop notation do resize.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("do-resize",
				     i18n_pspec("crop do resize"),
				     i18n_pspec("Crop the notation by do resize"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_RESIZE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) crop_note;

  task->launch = ags_crop_note_launch;
}

void
ags_crop_note_init(AgsCropNote *crop_note)
{
  crop_note->audio = NULL;
  crop_note->notation = NULL;

  crop_note->selection = NULL;
  
  crop_note->x_padding = 0;
  crop_note->x_crop = 0;

  crop_note->absolute = FALSE;
  crop_note->in_place = FALSE;
  crop_note->do_resize = FALSE;
}

void
ags_crop_note_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsCropNote *crop_note;

  crop_note = AGS_CROP_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(crop_note->audio == audio){
	return;
      }

      if(crop_note->audio != NULL){
	g_object_unref(crop_note->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      crop_note->audio = audio;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(crop_note->notation == notation){
	return;
      }

      if(crop_note->notation != NULL){
	g_object_unref(crop_note->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      crop_note->notation = notation;
    }
    break;
  case PROP_SELECTION:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_pointer(value);

      if(note == NULL ||
	 g_list_find(crop_note->selection, note) != NULL){
	return;
      }

      g_object_ref(note);
      crop_note->selection = g_list_prepend(crop_note->selection,
					    note);
    }
    break;
  case PROP_X_PADDING:
    {
      crop_note->x_padding = g_value_get_uint(value);
    }
    break;
  case PROP_X_CROP:
    {
      crop_note->x_crop = g_value_get_int(value);
    }
    break;
  case PROP_ABSOLUTE:
    {
      crop_note->absolute = g_value_get_boolean(value);
    }
    break;
  case PROP_IN_PLACE:
    {
      crop_note->in_place = g_value_get_boolean(value);
    }
    break;
  case PROP_DO_RESIZE:
    {
      crop_note->do_resize = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsCropNote *crop_note;

  crop_note = AGS_CROP_NOTE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, crop_note->audio);
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, crop_note->notation);
    }
    break;
  case PROP_SELECTION:
    {
      g_value_set_pointer(value,
			  g_list_copy_deep(crop_note->selection,
					   (GCopyFunc) g_object_ref,
					   NULL));
    }
    break;
  case PROP_X_PADDING:
    {
      g_value_set_uint(value, crop_note->x_padding);
    }
    break;
  case PROP_X_CROP:
    {
      g_value_set_int(value, crop_note->x_crop);
    }
    break;
  case PROP_ABSOLUTE:
    {
      g_value_set_boolean(value, crop_note->absolute);
    }
    break;
  case PROP_IN_PLACE:
    {
      g_value_set_boolean(value, crop_note->in_place);
    }
    break;
  case PROP_DO_RESIZE:
    {
      g_value_set_boolean(value, crop_note->do_resize);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_dispose(GObject *gobject)
{
  AgsCropNote *crop_note;

  crop_note = AGS_CROP_NOTE(gobject);

  if(crop_note->audio != NULL){
    g_object_unref(crop_note->audio);

    crop_note->audio = NULL;
  }

  if(crop_note->notation != NULL){
    g_object_unref(crop_note->notation);

    crop_note->notation = NULL;
  }

  if(crop_note->selection != NULL){
    g_list_free_full(crop_note->selection,
		     g_object_unref);

    crop_note->selection = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_crop_note_parent_class)->dispose(gobject);
}

void
ags_crop_note_finalize(GObject *gobject)
{
  AgsCropNote *crop_note;

  crop_note = AGS_CROP_NOTE(gobject);

  if(crop_note->audio != NULL){
    g_object_unref(crop_note->audio);
  }

  if(crop_note->notation != NULL){
    g_object_unref(crop_note->notation);
  }

  if(crop_note->selection != NULL){
    g_list_free_full(crop_note->selection,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_crop_note_parent_class)->finalize(gobject);
}

void
ags_crop_note_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsNotation *notation, *current_notation;
  AgsNote *note;

  AgsCropNote *crop_note;
  
  GList *selection;

  guint audio_channel;
  guint x_padding;
  gint x_crop;
  gint x_offset, x_prev;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  gboolean initial_run;

  crop_note = AGS_CROP_NOTE(task);

  /* get some properties */
  notation =
    current_notation = crop_note->notation;

  g_object_get(notation,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       NULL);
  
  selection = crop_note->selection;

  x_padding = crop_note->x_padding;
  x_crop = crop_note->x_crop;

  absolute = crop_note->absolute;

  in_place = crop_note->in_place;
  do_resize = crop_note->do_resize;

  /* crop */
  x_offset = 0;
  x_prev = 0;

  initial_run = TRUE;

  while(selection != NULL){
    note = ags_note_duplicate(AGS_NOTE(selection->data));
    
    if(absolute){
      if(in_place){
	note->x[1] = note->x[0] + x_crop;
      }else if(do_resize){
	if(initial_run){
	  x_offset = 0;
	  x_prev = note->x[0];

	  initial_run = FALSE;
	}else{
	  if(note->x[0] > x_prev){
	    x_offset += (note->x[0] - x_prev) * x_padding;
	    x_prev = note->x[0];
	  }	  
	}
	
	note->x[0] = x_offset + note->x[0];
	note->x[1] = x_offset + note->x[0] + x_crop;
      }
    }else{
      if(in_place){
	note->x[1] = note->x[1] + x_crop;
      }else if(do_resize){
	if(initial_run){
	  x_offset = 0;
	  x_prev = note->x[0];

	  initial_run = FALSE;
	}else{
	  if(note->x[0] > x_prev){
	    x_offset += (note->x[0] - x_prev) * x_padding;
	    x_prev = note->x[0];
	  }	  
	}

	note->x[0] = x_offset + note->x[0];
	note->x[1] = x_offset + note->x[1] + x_crop;
      }
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
 * ags_crop_note_new:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 * @selection: (element-type AgsAudio.Note) (transfer none): the selection as #GList-struct
 * @x_padding: the x padding to use
 * @x_crop: the amout to crop
 * @absolute: if %TRUE from absolute position, otherwise relative
 * @in_place: if %TRUE crop in place, otherwise grow relative offset
 * @do_resize: if %TRUE resize notation, otherwise not
 *
 * WARNING you need to provide #AgsAudio as a property.
 * Creates an #AgsCropNote task. Note either @in_place or @do_resize shall
 * be %TRUE else it won't have any effect.
 *
 * Returns: a new #AgsCropNote
 *
 * Since: 3.0.0
 */
AgsCropNote*
ags_crop_note_new(AgsAudio *audio,
		  AgsNotation *notation,
		  GList *selection,
		  guint x_padding, gint x_crop,
		  gboolean absolute, gboolean in_place, gboolean do_resize)
{
  AgsCropNote *crop_note;
  
  crop_note = (AgsCropNote *) g_object_new(AGS_TYPE_CROP_NOTE,
					   "audio", audio,
					   "notation", notation,
					   "x-padding", x_padding,
					   "x-crop", x_crop,
					   "absolute", absolute,
					   "in-place", in_place,
					   "do-resize", do_resize,
					   NULL);

  //FIXME:JK: argh, introspection!
  
  crop_note->selection = g_list_copy_deep(selection,
					  (GCopyFunc) g_object_ref,
					  NULL);
  
  return(crop_note);
}
