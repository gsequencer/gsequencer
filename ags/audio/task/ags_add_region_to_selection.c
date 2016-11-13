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

#include <ags/audio/task/ags_add_region_to_selection.h>

#include <ags/object/ags_connectable.h>

void ags_add_region_to_selection_class_init(AgsAddRegionToSelectionClass *add_region_to_selection);
void ags_add_region_to_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_region_to_selection_init(AgsAddRegionToSelection *add_region_to_selection);
void ags_add_region_to_selection_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_add_region_to_selection_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_add_region_to_selection_connect(AgsConnectable *connectable);
void ags_add_region_to_selection_disconnect(AgsConnectable *connectable);
void ags_add_region_to_selection_finalize(GObject *gobject);

void ags_add_region_to_selection_launch(AgsTask *task);

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_X0,
  PROP_X1,
  PROP_Y0,
  PROP_Y1,
  PROP_REPLACE_CURRENT_SELECTION,
};

/**
 * SECTION:ags_add_region_to_selection
 * @short_description: add region to notation selection
 * @title: AgsAddRegionToSelection
 * @section_id:
 * @include: ags/audio/task/ags_add_region_to_selection.h
 *
 * The #AgsAddRegionToSelection task adds the specified #AgsNote to selection of #AgsNotation.
 */

static gpointer ags_add_region_to_selection_parent_class = NULL;
static AgsConnectableInterface *ags_add_region_to_selection_parent_connectable_interface;

GType
ags_add_region_to_selection_get_type()
{
  static GType ags_type_add_region_to_selection = 0;

  if(!ags_type_add_region_to_selection){
    static const GTypeInfo ags_add_region_to_selection_info = {
      sizeof (AgsAddRegionToSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_region_to_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddRegionToSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_region_to_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_region_to_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_region_to_selection = g_type_register_static(AGS_TYPE_TASK,
							      "AgsAddRegionToSelection\0",
							      &ags_add_region_to_selection_info,
							      0);

    g_type_add_interface_static(ags_type_add_region_to_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_region_to_selection);
}

void
ags_add_region_to_selection_class_init(AgsAddRegionToSelectionClass *add_region_to_selection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_add_region_to_selection_parent_class = g_type_class_peek_parent(add_region_to_selection);

  /* gobject */
  gobject = (GObjectClass *) add_region_to_selection;

  gobject->set_property = ags_add_region_to_selection_set_property;
  gobject->get_property = ags_add_region_to_selection_get_property;

  gobject->finalize = ags_add_region_to_selection_finalize;

  /* properties */
  /**
   * AgsAddRegionToSelection:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("notation\0",
				   "notation of add note\0",
				   "The notation of add note task\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
  
  /**
   * AgsAddRegionToSelection:x0:
   *
   * Note offset x0.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("x0\0",
				 "offset x0\0",
				 "The x0 offset\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X0,
				  param_spec);
  /**
   * AgsAddRegionToSelection:x1:
   *
   * Note offset x1.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("x1\0",
				 "offset x1\0",
				 "The x1 offset\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X1,
				  param_spec);

  /**
   * AgsAddRegionToSelection:y0:
   *
   * Note offset y0.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("y0\0",
				 "offset y0\0",
				 "The y0 offset\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y0,
				  param_spec);

  /**
   * AgsAddRegionToSelection:y1:
   *
   * Note offset y1.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("y1\0",
				 "offset y1\0",
				 "The y1 offset\0",
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y1,
				  param_spec);

  /**
   * AgsAddRegionToSelection:replace-current-selection:
   *
   * The notation's replace-current-selection.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("replace-current-selection\0",
				     "replace current selection\0",
				     "Replace current selection of notation\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REPLACE_CURRENT_SELECTION,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) add_region_to_selection;

  task->launch = ags_add_region_to_selection_launch;
}

void
ags_add_region_to_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_region_to_selection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_region_to_selection_connect;
  connectable->disconnect = ags_add_region_to_selection_disconnect;
}

void
ags_add_region_to_selection_init(AgsAddRegionToSelection *add_region_to_selection)
{
  add_region_to_selection->notation = NULL;
  add_region_to_selection->x0 = 0;
  add_region_to_selection->y0 = 0;
  add_region_to_selection->x1 = 0;
  add_region_to_selection->y1 = 0;
  add_region_to_selection->replace_current_selection = TRUE;
}

void
ags_add_region_to_selection_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddRegionToSelection *add_region_to_selection;

  add_region_to_selection = AGS_ADD_REGION_TO_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(add_region_to_selection->notation == (GObject *) notation){
	return;
      }

      if(add_region_to_selection->notation != NULL){
	g_object_unref(add_region_to_selection->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      add_region_to_selection->notation = (GObject *) notation;
    }
    break;
  case PROP_X0:
    {
      add_region_to_selection->x0 = g_value_get_uint(value);
    }
  break;
  case PROP_X1:
    {
      add_region_to_selection->x1 = g_value_get_uint(value);
    }
  break;
  case PROP_Y0:
    {
      add_region_to_selection->y0 = g_value_get_uint(value);
    }
  break;
  case PROP_Y1:
    {
      add_region_to_selection->y1 = g_value_get_uint(value);
    }
  break;
  case PROP_REPLACE_CURRENT_SELECTION:
    {
      add_region_to_selection->replace_current_selection = g_value_get_boolean(value);
    }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_region_to_selection_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddRegionToSelection *add_region_to_selection;

  add_region_to_selection = AGS_ADD_REGION_TO_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, add_region_to_selection->notation);
    }
    break;
  case PROP_X0:
    {
      g_value_set_uint(value, add_region_to_selection->x0);
    }
    break;
  case PROP_X1:
    {
      g_value_set_uint(value, add_region_to_selection->x1);
    }
    break;
  case PROP_Y0:
    {
      g_value_set_uint(value, add_region_to_selection->y0);
    }
    break;
  case PROP_Y1:
    {
      g_value_set_uint(value, add_region_to_selection->y1);
    }
    break;
  case PROP_REPLACE_CURRENT_SELECTION:
    {
      g_value_set_boolean(value, add_region_to_selection->replace_current_selection);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_region_to_selection_connect(AgsConnectable *connectable)
{
  ags_add_region_to_selection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_region_to_selection_disconnect(AgsConnectable *connectable)
{
  ags_add_region_to_selection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_region_to_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_region_to_selection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_region_to_selection_launch(AgsTask *task)
{
  AgsAddRegionToSelection *add_region_to_selection;

  add_region_to_selection = AGS_ADD_REGION_TO_SELECTION(task);

  /* add note */
  ags_notation_add_region_to_selection(add_region_to_selection->notation,
				       add_region_to_selection->x0, add_region_to_selection->y0,
				       add_region_to_selection->x1, add_region_to_selection->y1,
				       add_region_to_selection->replace_current_selection);
}

/**
 * ags_add_region_to_selection_new:
 * @notation: the #AgsNotation providing the selection
 * @x0: start x coordinate
 * @y0: start y coordinate
 * @x1: end x coordinate
 * @y1: end y coordinate
 * @replace_current_selection: if %TRUE new selection is created, otherwise added to current
 *
 * Creates an #AgsAddRegionToSelection.
 *
 * Returns: an new #AgsAddRegionToSelection.
 *
 * Since: 0.4
 */
AgsAddRegionToSelection*
ags_add_region_to_selection_new(AgsNotation *notation,
				guint x0, guint y0,
				guint x1, guint y1,
				gboolean replace_current_selection)
{
  AgsAddRegionToSelection *add_region_to_selection;

  add_region_to_selection = (AgsAddRegionToSelection *) g_object_new(AGS_TYPE_ADD_REGION_TO_SELECTION,
								     NULL);

  add_region_to_selection->notation = notation;
  add_region_to_selection->x0 = x0;
  add_region_to_selection->y0 = y0;
  add_region_to_selection->x1 = x1;
  add_region_to_selection->y1 = y1;
  add_region_to_selection->replace_current_selection = replace_current_selection;

  return(add_region_to_selection);
}
