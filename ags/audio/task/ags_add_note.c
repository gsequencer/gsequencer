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

#include <ags/audio/task/ags_add_note.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_add_note_class_init(AgsAddNoteClass *add_note);
void ags_add_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_note_init(AgsAddNote *add_note);
void ags_add_note_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_add_note_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_add_note_connect(AgsConnectable *connectable);
void ags_add_note_disconnect(AgsConnectable *connectable);
void ags_add_note_finalize(GObject *gobject);

void ags_add_note_launch(AgsTask *task);

/**
 * SECTION:ags_add_note
 * @short_description: add note object to notation
 * @title: AgsAddNote
 * @section_id:
 * @include: ags/audio/task/ags_add_note.h
 *
 * The #AgsAddNote task adds #AgsNote to #AgsNotation.
 */

static gpointer ags_add_note_parent_class = NULL;
static AgsConnectableInterface *ags_add_note_parent_connectable_interface;

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_NOTE,
  PROP_USE_SELECTION_LIST,
};

GType
ags_add_note_get_type()
{
  static GType ags_type_add_note = 0;

  if(!ags_type_add_note){
    static const GTypeInfo ags_add_note_info = {
      sizeof (AgsAddNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_note = g_type_register_static(AGS_TYPE_TASK,
					       "AgsAddNote",
					       &ags_add_note_info,
					       0);

    g_type_add_interface_static(ags_type_add_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_note);
}

void
ags_add_note_class_init(AgsAddNoteClass *add_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_add_note_parent_class = g_type_class_peek_parent(add_note);

  /* gobject */
  gobject = (GObjectClass *) add_note;

  gobject->set_property = ags_add_note_set_property;
  gobject->get_property = ags_add_note_get_property;

  gobject->finalize = ags_add_note_finalize;

  /* properties */
  /**
   * AgsAddNote:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 0.7.117
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
   * AgsAddNote:note:
   *
   * The assigned #AgsNote
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("note",
				   i18n_pspec("note of add note"),
				   i18n_pspec("The note of add note task"),
				   AGS_TYPE_NOTE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /**
   * AgsAddNote:use-selection-list:
   *
   * The notation's use-selection-list.
   * 
   * Since: 0.7.117
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
  task = (AgsTaskClass *) add_note;

  task->launch = ags_add_note_launch;
}

void
ags_add_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_note_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_note_connect;
  connectable->disconnect = ags_add_note_disconnect;
}

void
ags_add_note_init(AgsAddNote *add_note)
{
  add_note->notation = NULL;
  add_note->note = NULL;
  add_note->use_selection_list = FALSE;
}

void
ags_add_note_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddNote *add_note;

  add_note = AGS_ADD_NOTE(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(add_note->notation == (GObject *) notation){
	return;
      }

      if(add_note->notation != NULL){
	g_object_unref(add_note->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      add_note->notation = (GObject *) notation;
    }
    break;
  case PROP_NOTE:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_object(value);

      if(add_note->note == (GObject *) note){
	return;
      }

      if(add_note->note != NULL){
	g_object_unref(add_note->note);
      }

      if(note != NULL){
	g_object_ref(note);
      }

      add_note->note = (GObject *) note;
    }
    break;
  case PROP_USE_SELECTION_LIST:
    {
      add_note->use_selection_list = g_value_get_boolean(value);
    }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_note_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddNote *add_note;

  add_note = AGS_ADD_NOTE(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, add_note->notation);
    }
    break;
  case PROP_NOTE:
    {
      g_value_set_object(value, add_note->note);
    }
    break;
  case PROP_USE_SELECTION_LIST:
    {
      g_value_set_boolean(value, add_note->use_selection_list);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_note_connect(AgsConnectable *connectable)
{
  ags_add_note_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_note_disconnect(AgsConnectable *connectable)
{
  ags_add_note_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_note_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_note_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_note_launch(AgsTask *task)
{
  AgsAddNote *add_note;

  add_note = AGS_ADD_NOTE(task);

  /* add note */
  ags_notation_add_note(add_note->notation,
			add_note->note,
			add_note->use_selection_list);
}

/**
 * ags_add_note_new:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to add
 * @use_selection_list: if %TRUE added to selection, otherwise to notation
 *
 * Creates an #AgsAddNote.
 *
 * Returns: an new #AgsAddNote.
 *
 * Since: 0.4
 */
AgsAddNote*
ags_add_note_new(AgsNotation *notation,
		 AgsNote *note,
		 gboolean use_selection_list)
{
  AgsAddNote *add_note;

  add_note = (AgsAddNote *) g_object_new(AGS_TYPE_ADD_NOTE,
					 NULL);

  add_note->notation = notation;
  add_note->note = note;
  add_note->use_selection_list = use_selection_list;

  return(add_note);
}
