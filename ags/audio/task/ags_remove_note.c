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

#include <ags/audio/task/ags_remove_note.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_remove_note_class_init(AgsRemoveNoteClass *remove_note);
void ags_remove_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_note_init(AgsRemoveNote *remove_note);
void ags_remove_note_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_remove_note_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_remove_note_connect(AgsConnectable *connectable);
void ags_remove_note_disconnect(AgsConnectable *connectable);
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
static AgsConnectableInterface *ags_remove_note_parent_connectable_interface;

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_X,
  PROP_Y,
};

GType
ags_remove_note_get_type()
{
  static GType ags_type_remove_note = 0;

  if(!ags_type_remove_note){
    static const GTypeInfo ags_remove_note_info = {
      sizeof (AgsRemoveNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_note = g_type_register_static(AGS_TYPE_TASK,
						  "AgsRemoveNote",
						  &ags_remove_note_info,
						  0);

    g_type_add_interface_static(ags_type_remove_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_note);
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
   * AgsRemoveNote:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("notation of add note"),
				   i18n_pspec("The notation of add note task"),
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /**
   * AgsRemoveNote:x:
   *
   * Note offset x.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("x",
				 i18n_pspec("offset x"),
				 i18n_pspec("The x offset"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsRemoveNote:y:
   *
   * Note offset y.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("y",
				 i18n_pspec("offset y"),
				 i18n_pspec("The y offset"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) remove_note;

  task->launch = ags_remove_note_launch;
}

void
ags_remove_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_note_connect;
  connectable->disconnect = ags_remove_note_disconnect;
}

void
ags_remove_note_init(AgsRemoveNote *remove_note)
{
  remove_note->notation = NULL;
  remove_note->x = 0;
  remove_note->y = 0;
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
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(remove_note->notation == (GObject *) notation){
	return;
      }

      if(remove_note->notation != NULL){
	g_object_unref(remove_note->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      remove_note->notation = (GObject *) notation;
    }
    break;
  case PROP_X:
    {
      remove_note->x = g_value_get_uint(value);
    }
    break;
  case PROP_Y:
    {
      remove_note->y = g_value_get_uint(value);
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
  case PROP_NOTATION:
    {
      g_value_set_object(value, remove_note->notation);
    }
    break;
  case PROP_X:
    {
      g_value_set_uint(value, remove_note->x);
    }
    break;
  case PROP_Y:
    {
      g_value_set_uint(value, remove_note->y);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_note_connect(AgsConnectable *connectable)
{
  ags_remove_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_note_disconnect(AgsConnectable *connectable)
{
  ags_remove_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_note_dispose(GObject *gobject)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  if(remove_note->notation != NULL){
    g_object_unref(remove_note->notation);

    remove_note->notation = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_remove_note_parent_class)->dispose(gobject);
}

void
ags_remove_note_finalize(GObject *gobject)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(gobject);

  if(remove_note->notation != NULL){
    g_object_unref(remove_note->notation);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_remove_note_parent_class)->finalize(gobject);
}

void
ags_remove_note_launch(AgsTask *task)
{
  AgsRemoveNote *remove_note;

  remove_note = AGS_REMOVE_NOTE(task);

  /* remove note */
  ags_notation_remove_note_at_position(remove_note->notation,
				       remove_note->x, remove_note->y);
}

/**
 * ags_remove_note_new:
 * @notation: the #AgsNotation
 * @x: the x coordinate of #AgsNote
 * @y: the y coordinate of #AgsNote
 *
 * Creates an #AgsRemoveNote.
 *
 * Returns: an new #AgsRemoveNote.
 *
 * Since: 1.0.0
 */
AgsRemoveNote*
ags_remove_note_new(AgsNotation *notation,
		    guint x, guint y)
{
  AgsRemoveNote *remove_note;

  remove_note = (AgsRemoveNote *) g_object_new(AGS_TYPE_REMOVE_NOTE,
					       "notation", notation,
					       "x", x,
					       "y", y,
					       NULL);

  return(remove_note);
}
