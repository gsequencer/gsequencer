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

#include <ags/audio/task/ags_add_point_to_selection.h>

#include <ags/object/ags_connectable.h>

void ags_add_point_to_selection_class_init(AgsAddPointToSelectionClass *add_point_to_selection);
void ags_add_point_to_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_point_to_selection_init(AgsAddPointToSelection *add_point_to_selection);
void ags_add_point_to_selection_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_add_point_to_selection_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_add_point_to_selection_connect(AgsConnectable *connectable);
void ags_add_point_to_selection_disconnect(AgsConnectable *connectable);
void ags_add_point_to_selection_finalize(GObject *gobject);

void ags_add_point_to_selection_launch(AgsTask *task);

/**
 * SECTION:ags_add_point_to_selection
 * @short_description: add point to notation selection
 * @title: AgsAddPointToSelection
 * @section_id:
 * @include: ags/audio/task/ags_add_point_to_selection.h
 *
 * The #AgsAddPointToSelection task adds the specified #AgsNote to selection of #AgsNotation.
 */

static gpointer ags_add_point_to_selection_parent_class = NULL;
static AgsConnectableInterface *ags_add_point_to_selection_parent_connectable_interface;

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_X,
  PROP_Y,
  PROP_REPLACE_CURRENT_SELECTION,
};

GType
ags_add_point_to_selection_get_type()
{
  static GType ags_type_add_point_to_selection = 0;

  if(!ags_type_add_point_to_selection){
    static const GTypeInfo ags_add_point_to_selection_info = {
      sizeof (AgsAddPointToSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_point_to_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddPointToSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_point_to_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_point_to_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_point_to_selection = g_type_register_static(AGS_TYPE_TASK,
							     "AgsAddPointToSelection",
							     &ags_add_point_to_selection_info,
							     0);

    g_type_add_interface_static(ags_type_add_point_to_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_point_to_selection);
}

void
ags_add_point_to_selection_class_init(AgsAddPointToSelectionClass *add_point_to_selection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_add_point_to_selection_parent_class = g_type_class_peek_parent(add_point_to_selection);

  /* gobject */
  gobject = (GObjectClass *) add_point_to_selection;

  gobject->set_property = ags_add_point_to_selection_set_property;
  gobject->get_property = ags_add_point_to_selection_get_property;

  gobject->finalize = ags_add_point_to_selection_finalize;

  /* properties */
  /**
   * AgsAddPointToSelection:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("notation",
				   "notation of add point to selection",
				   "The notation of add point to selection task",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
  
  /**
   * AgsAddPointToSelection:x:
   *
   * Note offset x.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("x",
				 "offset x",
				 "The x offset",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsAddPointToSelection:y:
   *
   * Note offset y.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("y",
				 "offset y",
				 "The y offset",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y,
				  param_spec);

  /**
   * AgsAddPointToSelection:replace-current-selection:
   *
   * The notation's replace-current-selection.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("replace-current-selection",
				     "replace current selection",
				     "Replace current selection of notation",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REPLACE_CURRENT_SELECTION,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) add_point_to_selection;

  task->launch = ags_add_point_to_selection_launch;
}

void
ags_add_point_to_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_point_to_selection_connect;
  connectable->disconnect = ags_add_point_to_selection_disconnect;
}

void
ags_add_point_to_selection_init(AgsAddPointToSelection *add_point_to_selection)
{
  add_point_to_selection->notation = NULL;
  add_point_to_selection->x = 0;
  add_point_to_selection->y = 0;
  add_point_to_selection->replace_current_selection = TRUE;
}

void
ags_add_point_to_selection_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = AGS_ADD_POINT_TO_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(add_point_to_selection->notation == (GObject *) notation){
	return;
      }

      if(add_point_to_selection->notation != NULL){
	g_object_unref(add_point_to_selection->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      add_point_to_selection->notation = (GObject *) notation;
    }
    break;
  case PROP_X:
    {
      add_point_to_selection->x = g_value_get_uint(value);
    }
    break;
  case PROP_Y:
    {
      add_point_to_selection->y = g_value_get_uint(value);
    }
    break;
  case PROP_REPLACE_CURRENT_SELECTION:
    {
      add_point_to_selection->replace_current_selection = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_point_to_selection_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = AGS_ADD_POINT_TO_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, add_point_to_selection->notation);
    }
    break;
  case PROP_X:
    {
      g_value_set_uint(value, add_point_to_selection->x);
    }
    break;
  case PROP_Y:
    {
      g_value_set_uint(value, add_point_to_selection->y);
    }
    break;
  case PROP_REPLACE_CURRENT_SELECTION:
    {
      g_value_set_boolean(value, add_point_to_selection->replace_current_selection);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_point_to_selection_connect(AgsConnectable *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_point_to_selection_disconnect(AgsConnectable *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_point_to_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_point_to_selection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_point_to_selection_launch(AgsTask *task)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = AGS_ADD_POINT_TO_SELECTION(task);

  /* add point to selection */
  ags_notation_add_point_to_selection(add_point_to_selection->notation,
				      add_point_to_selection->x, add_point_to_selection->y,
				      add_point_to_selection->replace_current_selection);
}

/**
 * ags_add_point_to_selection_new:
 * @notation: the #AgsNotation providing the selection
 * @x: x coordinate
 * @y: y coordinate
 * @replace_current_selection: if %TRUE new selection is created, otherwise added to current
 *
 * Creates an #AgsAddPointToSelection.
 *
 * Returns: an new #AgsAddPointToSelection.
 *
 * Since: 0.4
 */
AgsAddPointToSelection*
ags_add_point_to_selection_new(AgsNotation *notation,
			       guint x, guint y,
			       gboolean replace_current_selection)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = (AgsAddPointToSelection *) g_object_new(AGS_TYPE_ADD_POINT_TO_SELECTION,
								   NULL);

  add_point_to_selection->notation = notation;
  add_point_to_selection->x = x;
  add_point_to_selection->y = y;
  add_point_to_selection->replace_current_selection = replace_current_selection;

  return(add_point_to_selection);
}
