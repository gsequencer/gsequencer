/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_crop_note_class_init(AgsCropNoteClass *crop_note);
void ags_crop_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_crop_note_init(AgsCropNote *crop_note);
void ags_crop_note_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_crop_note_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_crop_note_connect(AgsConnectable *connectable);
void ags_crop_note_disconnect(AgsConnectable *connectable);
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
static AgsConnectableInterface *ags_crop_note_parent_connectable_interface;

enum{
  PROP_0,
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
  static GType ags_type_crop_note = 0;

  if(!ags_type_crop_note){
    static const GTypeInfo ags_crop_note_info = {
      sizeof (AgsCropNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCropNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_crop_note = g_type_register_static(AGS_TYPE_TASK,
					       "AgsCropNote",
					       &ags_crop_note_info,
					       0);

    g_type_add_interface_static(ags_type_crop_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_crop_note);
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

  gobject->finalize = ags_crop_note_finalize;

  /* properties */
  /**
   * AgsCropNote:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 0.8.9
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
   * Since: 0.8.9
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
   * Since: 0.8.9
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
   * Since: 0.8.9
   */
  param_spec =  g_param_spec_uint("x-crop",
				  i18n_pspec("crop with x-crop amount"),
				  i18n_pspec("Crop the notation by x-crop amount"),
				  0,
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
   * Since: 0.8.9
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
   * Since: 0.8.9
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
   * Since: 0.8.9
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
ags_crop_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_crop_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_crop_note_connect;
  connectable->disconnect = ags_crop_note_disconnect;
}

void
ags_crop_note_init(AgsCropNote *crop_note)
{
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
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(crop_note->notation == (GObject *) notation){
	return;
      }

      if(crop_note->notation != NULL){
	g_object_unref(crop_note->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      crop_note->notation = (GObject *) notation;
    }
    break;
  case PROP_SELECTION:
    {
      GList *selection;

      selection = (GList *) g_value_get_pointer(value);

      if(crop_note->selection == selection){
	return;
      }

      if(crop_note->selection != NULL){
	g_list_free(crop_note->selection);
      }
      
      crop_note->selection = g_list_copy(selection);
    }
    break;
  case PROP_X_PADDING:
    {
      crop_note->x_padding = g_value_get_uint(value);
    }
  break;
  case PROP_X_CROP:
    {
      crop_note->x_crop = g_value_get_uint(value);
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
  case PROP_NOTATION:
    {
      g_value_set_object(value, crop_note->notation);
    }
    break;
  case PROP_SELECTION:
    {
      g_value_set_pointer(value,
			  g_list_copy(crop_note->selection));
    }
    break;
  case PROP_X_PADDING:
    {
      g_value_set_uint(value, crop_note->x_padding);
    }
    break;
  case PROP_X_CROP:
    {
      g_value_set_uint(value, crop_note->x_crop);
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
ags_crop_note_connect(AgsConnectable *connectable)
{
  ags_crop_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_crop_note_disconnect(AgsConnectable *connectable)
{
  ags_crop_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_crop_note_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_crop_note_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_crop_note_launch(AgsTask *task)
{
  AgsCropNote *crop_note;

  AgsNotation *notation;
  
  GList *selection;

  guint x_padding;
  guint x_crop;
  gint x_offset, x_prev;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  gboolean initial_run;
  
  crop_note = AGS_CROP_NOTE(task);

  /* get some properties */
  notation = crop_note->notation;

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
    if(absolute){
      if(in_place){
	AGS_NOTE(selection->data)->x[1] = AGS_NOTE(selection->data)->x[0] + x_crop;
      }else if(do_resize){
	if(initial_run){
	  x_offset = 0;
	  x_prev = AGS_NOTE(selection->data)->x[0];

	  initial_run = FALSE;
	}else{
	  if(AGS_NOTE(selection->data)->x[0] > x_prev){
	    x_offset += (AGS_NOTE(selection->data)->x[0] - x_prev) * x_padding;
	    x_prev = AGS_NOTE(selection->data)->x[0];
	  }	  
	}
	
	AGS_NOTE(selection->data)->x[0] = x_offset + AGS_NOTE(selection->data)->x[0];
	AGS_NOTE(selection->data)->x[1] = x_offset + AGS_NOTE(selection->data)->x[0] + x_crop;
      }
    }else{
      if(in_place){
	AGS_NOTE(selection->data)->x[1] = AGS_NOTE(selection->data)->x[1] + x_crop;
      }else if(do_resize){
	if(initial_run){
	  x_offset = 0;
	  x_prev = AGS_NOTE(selection->data)->x[0];

	  initial_run = FALSE;
	}else{
	  if(AGS_NOTE(selection->data)->x[0] > x_prev){
	    x_offset += (AGS_NOTE(selection->data)->x[0] - x_prev) * x_padding;
	    x_prev = AGS_NOTE(selection->data)->x[0];
	  }	  
	}

	AGS_NOTE(selection->data)->x[0] = x_offset + AGS_NOTE(selection->data)->x[0];
	AGS_NOTE(selection->data)->x[1] = x_offset + AGS_NOTE(selection->data)->x[1] + x_crop;
      }
    }

    selection = selection->next;
  }
}

/**
 * ags_crop_note_new:
 * @notation: the #AgsNotation
 * @selection: the selection as #GList-struct
 * @x_padding: the x padding to use
 * @x_crop: the amout to crop
 * @absolute: if %TRUE from absolute position, otherwise relative
 * @in_place: if %TRUE crop in place, otherwise grow relative offset
 * @do_resize: if %TRUE resize notation, otherwise not
 *
 * Creates an #AgsCropNote task. Note either @in_place or @do_resize shall
 * be %TRUE else it won't have any effect.
 *
 * Returns: a new #AgsCropNote
 *
 * Since: 0.8.9
 */
AgsCropNote*
ags_crop_note_new(AgsNotation *notation,
		  GList *selection,
		  guint x_padding, guint x_crop,
		  gboolean absolute, gboolean in_place, gboolean do_resize)
{
  AgsCropNote *crop_note;

  crop_note = (AgsCropNote *) g_object_new(AGS_TYPE_CROP_NOTE,
					   "notation", notation,
					   "selection", selection,
					   "x-padding", x_padding,
					   "x-crop", x_crop,
					   "absolute", absolute,
					   "in-place", in_place,
					   "do-resize", do_resize,
					   NULL);

  return(crop_note);
}
