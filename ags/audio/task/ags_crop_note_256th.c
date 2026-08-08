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

#include <ags/audio/task/ags_crop_note_256th.h>

#include <ags/i18n.h>

#include <math.h>

static void ags_crop_note_256th_class_init(AgsCropNote256thClass *crop_note_256th);
static void ags_crop_note_256th_init(AgsCropNote256th *crop_note_256th);
static void ags_crop_note_256th_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
static void ags_crop_note_256th_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
static void ags_crop_note_256th_dispose(GObject *gobject);
static void ags_crop_note_256th_finalize(GObject *gobject);

static void ags_crop_note_256th_launch(AgsTask *task);

/**
 * SECTION:ags_crop_note_256th
 * @short_description: crop notation
 * @title: AgsCropNote256th
 * @section_id:
 * @include: ags/audio/task/ags_crop_note_256th.h
 *
 * The #AgsCropNote256th task crops #AgsNotation.
 */

static gpointer ags_crop_note_256th_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_NOTATION,
  PROP_SELECTION,
  PROP_X_256TH_PADDING,
  PROP_X_256TH_CROP,
  PROP_ABSOLUTE,
  PROP_IN_PLACE,
  PROP_DO_RESIZE,
};

GType
ags_crop_note_256th_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_crop_note_256th = 0;

    static const GTypeInfo ags_crop_note_256th_info = {
      sizeof(AgsCropNote256thClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_256th_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCropNote256th),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_256th_init,
    };

    ags_type_crop_note_256th = g_type_register_static(AGS_TYPE_TASK,
						      "AgsCropNote256th",
						      &ags_crop_note_256th_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_crop_note_256th);
  }

  return(g_define_type_id__static);
}

void
ags_crop_note_256th_class_init(AgsCropNote256thClass *crop_note_256th)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_crop_note_256th_parent_class = g_type_class_peek_parent(crop_note_256th);

  /* gobject */
  gobject = (GObjectClass *) crop_note_256th;

  gobject->set_property = ags_crop_note_256th_set_property;
  gobject->get_property = ags_crop_note_256th_get_property;

  gobject->dispose = ags_crop_note_256th_dispose;
  gobject->finalize = ags_crop_note_256th_finalize;

  /* properties */
  /**
   * AgsCropNote256th:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 9.1.3
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
   * AgsCropNote256th:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 9.1.3
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
   * AgsCropNote256th:selection: (type GList(AgsNote)) (transfer full)
   *
   * The assigned #AgsNote
   * 
   * Since: 9.1.3
   */
  param_spec = g_param_spec_pointer("selection",
				    i18n_pspec("selection to crop"),
				    i18n_pspec("The selection to crop"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION,
				  param_spec);

  /**
   * AgsCropNote256th:x-256th-padding:
   *
   * Crop notation with x 256th padding.
   * 
   * Since: 9.1.3
   */
  param_spec =  g_param_spec_uint("x-256th-padding",
				  i18n_pspec("crop with x 256th padding"),
				  i18n_pspec("Crop the notation with x 256th padding"),
				  0,
				  AGS_CROP_NOTE_256TH_DEFAULT_X_LENGTH,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_256TH_PADDING,
				  param_spec);

  /**
   * AgsCropNote256th:x-256th-crop:
   *
   * Crop notation by x 256th crop amount.
   * 
   * Since: 9.1.3
   */
  param_spec =  g_param_spec_int("x-256th-crop",
				 i18n_pspec("crop with x 256th crop amount"),
				 i18n_pspec("Crop the notation by x 256th crop amount"),
				 -1 * AGS_CROP_NOTE_256TH_DEFAULT_X_LENGTH,
				 AGS_CROP_NOTE_256TH_DEFAULT_X_LENGTH,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_256TH_CROP,
				  param_spec);

  /**
   * AgsCropNote256th:absolute:
   *
   * Crop notation by absolute position.
   * 
   * Since: 9.1.3
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
   * AgsCropNote256th:in-place:
   *
   * Crop notation in place.
   * 
   * Since: 9.1.3
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
   * AgsCropNote256th:do-resize:
   *
   * Crop notation do resize.
   * 
   * Since: 9.1.3
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
  task = (AgsTaskClass *) crop_note_256th;

  task->launch = ags_crop_note_256th_launch;
}

void
ags_crop_note_256th_init(AgsCropNote256th *crop_note_256th)
{
  crop_note_256th->audio = NULL;
  crop_note_256th->notation = NULL;

  crop_note_256th->selection = NULL;
  
  crop_note_256th->x_256th_padding = 0;
  crop_note_256th->x_256th_crop = 0;

  crop_note_256th->absolute = FALSE;
  crop_note_256th->in_place = FALSE;
  crop_note_256th->do_resize = FALSE;
}

void
ags_crop_note_256th_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsCropNote256th *crop_note_256th;

  crop_note_256th = AGS_CROP_NOTE_256TH(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(crop_note_256th->audio == audio){
	return;
      }

      if(crop_note_256th->audio != NULL){
	g_object_unref(crop_note_256th->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      crop_note_256th->audio = audio;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(crop_note_256th->notation == notation){
	return;
      }

      if(crop_note_256th->notation != NULL){
	g_object_unref(crop_note_256th->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      crop_note_256th->notation = notation;
    }
    break;
  case PROP_SELECTION:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_pointer(value);

      if(note == NULL ||
	 g_list_find(crop_note_256th->selection, note) != NULL){
	return;
      }

      g_object_ref(note);
      crop_note_256th->selection = g_list_prepend(crop_note_256th->selection,
						  note);
    }
    break;
  case PROP_X_256TH_PADDING:
    {
      crop_note_256th->x_256th_padding = g_value_get_uint(value);
    }
    break;
  case PROP_X_256TH_CROP:
    {
      crop_note_256th->x_256th_crop = g_value_get_int(value);
    }
    break;
  case PROP_ABSOLUTE:
    {
      crop_note_256th->absolute = g_value_get_boolean(value);
    }
    break;
  case PROP_IN_PLACE:
    {
      crop_note_256th->in_place = g_value_get_boolean(value);
    }
    break;
  case PROP_DO_RESIZE:
    {
      crop_note_256th->do_resize = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_256th_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsCropNote256th *crop_note_256th;

  crop_note_256th = AGS_CROP_NOTE_256TH(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, crop_note_256th->audio);
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, crop_note_256th->notation);
    }
    break;
  case PROP_SELECTION:
    {
      g_value_set_pointer(value,
			  g_list_copy_deep(crop_note_256th->selection,
					   (GCopyFunc) g_object_ref,
					   NULL));
    }
    break;
  case PROP_X_256TH_PADDING:
    {
      g_value_set_uint(value, crop_note_256th->x_256th_padding);
    }
    break;
  case PROP_X_256TH_CROP:
    {
      g_value_set_int(value, crop_note_256th->x_256th_crop);
    }
    break;
  case PROP_ABSOLUTE:
    {
      g_value_set_boolean(value, crop_note_256th->absolute);
    }
    break;
  case PROP_IN_PLACE:
    {
      g_value_set_boolean(value, crop_note_256th->in_place);
    }
    break;
  case PROP_DO_RESIZE:
    {
      g_value_set_boolean(value, crop_note_256th->do_resize);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_256th_dispose(GObject *gobject)
{
  AgsCropNote256th *crop_note_256th;

  crop_note_256th = AGS_CROP_NOTE_256TH(gobject);

  if(crop_note_256th->audio != NULL){
    g_object_unref(crop_note_256th->audio);

    crop_note_256th->audio = NULL;
  }

  if(crop_note_256th->notation != NULL){
    g_object_unref(crop_note_256th->notation);

    crop_note_256th->notation = NULL;
  }

  if(crop_note_256th->selection != NULL){
    g_list_free_full(crop_note_256th->selection,
		     g_object_unref);

    crop_note_256th->selection = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_crop_note_256th_parent_class)->dispose(gobject);
}

void
ags_crop_note_256th_finalize(GObject *gobject)
{
  AgsCropNote256th *crop_note_256th;

  crop_note_256th = AGS_CROP_NOTE_256TH(gobject);

  if(crop_note_256th->audio != NULL){
    g_object_unref(crop_note_256th->audio);
  }

  if(crop_note_256th->notation != NULL){
    g_object_unref(crop_note_256th->notation);
  }

  if(crop_note_256th->selection != NULL){
    g_list_free_full(crop_note_256th->selection,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_crop_note_256th_parent_class)->finalize(gobject);
}

void
ags_crop_note_256th_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsNotation *notation, *current_notation;
  AgsNote *note;

  AgsCropNote256th *crop_note_256th;
  
  GList *selection;

  guint audio_channel;
  guint x_256th_padding;
  gint x_256th_crop;
  gint x_256th_offset, x_256th_prev;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  gboolean initial_run;

  crop_note_256th = AGS_CROP_NOTE_256TH(task);

  g_return_if_fail(AGS_IS_AUDIO(crop_note_256th->audio));
  g_return_if_fail(AGS_IS_NOTATION(crop_note_256th->notation));

  g_object_get(crop_note_256th,
	       "audio", &audio,
	       NULL);
  
  /* get some properties */
  notation =
    current_notation = crop_note_256th->notation;

  g_object_get(notation,
	       "audio-channel", &audio_channel,
	       NULL);
  
  selection = crop_note_256th->selection;

  x_256th_padding = crop_note_256th->x_256th_padding;
  x_256th_crop = crop_note_256th->x_256th_crop;

  absolute = crop_note_256th->absolute;

  in_place = crop_note_256th->in_place;
  do_resize = crop_note_256th->do_resize;

  /* crop */
  x_256th_offset = 0;
  x_256th_prev = 0;

  initial_run = TRUE;

  while(selection != NULL){
    note = ags_note_duplicate(AGS_NOTE(selection->data));
    
    if(absolute){
      if(in_place){
	note->x[1] = note->x[0] + (x_256th_crop / 16);

	note->x_256th[1] = note->x_256th[0] + (x_256th_crop);
      }else if(do_resize){
	if(initial_run){
	  x_256th_offset = 0;
	  x_256th_prev = note->x_256th[0];

	  initial_run = FALSE;
	}else{
	  if(note->x_256th[0] > x_256th_prev){
	    x_256th_offset += (note->x_256th[0] - x_256th_prev) * x_256th_padding;
	    x_256th_prev = note->x_256th[0];
	  }	  
	}
	
	note->x[0] = (x_256th_offset / 16) + note->x[0];
	note->x[1] = (x_256th_offset / 16) + note->x[0] + (x_256th_crop / 16);

	note->x_256th[0] = (x_256th_offset) + note->x_256th[0];
	note->x_256th[1] = (x_256th_offset) + note->x_256th[0] + (x_256th_crop);
      }
    }else{
      if(in_place){
	note->x[1] = note->x[1] + (x_256th_crop / 16);

	note->x_256th[1] = note->x_256th[1] + (x_256th_crop);
      }else if(do_resize){
	if(initial_run){
	  x_256th_offset = 0;
	  x_256th_prev = note->x_256th[0];

	  initial_run = FALSE;
	}else{
	  if(note->x_256th[0] > x_256th_prev){
	    x_256th_offset += (note->x_256th[0] - x_256th_prev) * x_256th_padding;
	    x_256th_prev = note->x_256th[0];
	  }	  
	}

	note->x[0] = (x_256th_offset / 16) + note->x[0];
	note->x[1] = (x_256th_offset / 16) + note->x[1] + (x_256th_crop / 16);

	note->x_256th[0] = (x_256th_offset) + note->x_256th[0];
	note->x_256th[1] = (x_256th_offset) + note->x_256th[1] + (x_256th_crop);
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
 * ags_crop_note_256th_set_selection:
 * @crop_note_256th: the #AgsCropNote256th
 * @selection: (element-type AgsAudio.Note256th) (transfer none): the selection as #GList-struct
 * 
 * Set @selection of @crop_note_256th.
 * 
 * Since: 3.2.3
 */
void
ags_crop_note_256th_set_selection(AgsCropNote256th *crop_note_256th,
				  GList *selection)
{
  if(!AGS_IS_CROP_NOTE_256TH(crop_note_256th) ||
     crop_note_256th->selection == selection){
    return;
  }

  if(crop_note_256th->selection != NULL){
    g_list_free_full(crop_note_256th->selection,
		     (GDestroyNotify) g_object_unref);
  }
  
  crop_note_256th->selection = g_list_copy_deep(selection,
						(GCopyFunc) g_object_ref,
						NULL);
}

/**
 * ags_crop_note_256th_new:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 * @selection: (element-type AgsAudio.Note256th) (transfer none): the selection as #GList-struct
 * @x_256th_padding: the x padding to use
 * @x_256th_crop: the amout to crop
 * @absolute: if %TRUE from absolute position, otherwise relative
 * @in_place: if %TRUE crop in place, otherwise grow relative offset
 * @do_resize: if %TRUE resize notation, otherwise not
 *
 * WARNING you need to provide #AgsAudio as a property.
 * Creates an #AgsCropNote256th task. Note256th either @in_place or @do_resize shall
 * be %TRUE else it won't have any effect.
 *
 * Returns: a new #AgsCropNote256th
 *
 * Since: 9.1.3
 */
AgsCropNote256th*
ags_crop_note_256th_new(AgsAudio *audio,
			AgsNotation *notation,
			GList *selection,
			guint x_256th_padding, gint x_256th_crop,
			gboolean absolute, gboolean in_place, gboolean do_resize)
{
  AgsCropNote256th *crop_note_256th;
  
  crop_note_256th = (AgsCropNote256th *) g_object_new(AGS_TYPE_CROP_NOTE_256TH,
						      "audio", audio,
						      "notation", notation,
						      "x-256th-padding", x_256th_padding,
						      "x-256th-crop", x_256th_crop,
						      "absolute", absolute,
						      "in-place", in_place,
						      "do-resize", do_resize,
						      NULL);

  ags_crop_note_256th_set_selection(crop_note_256th,
				    selection);
  
  return(crop_note_256th);
}
