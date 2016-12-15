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

#include <ags/audio/task/ags_remove_region_from_selection.h>

#include <ags/object/ags_connectable.h>

void ags_remove_region_from_selection_class_init(AgsRemoveRegionFromSelectionClass *remove_region_from_selection);
void ags_remove_region_from_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_region_from_selection_init(AgsRemoveRegionFromSelection *remove_region_from_selection);
void ags_remove_region_from_selection_set_property(GObject *gobject,
						   guint prop_id,
						   const GValue *value,
						   GParamSpec *param_spec);
void ags_remove_region_from_selection_get_property(GObject *gobject,
						   guint prop_id,
						   GValue *value,
						   GParamSpec *param_spec);
void ags_remove_region_from_selection_connect(AgsConnectable *connectable);
void ags_remove_region_from_selection_disconnect(AgsConnectable *connectable);
void ags_remove_region_from_selection_finalize(GObject *gobject);

void ags_remove_region_from_selection_launch(AgsTask *task);

/**
 * SECTION:ags_remove_region_from_selection
 * @short_description: remove region from notation selection
 * @title: AgsRemoveRegionFromSelection
 * @section_id:
 * @include: ags/audio/task/ags_remove_region_from_selection.h
 *
 * The #AgsRemoveRegionFromSelection task removes the specified #AgsNote from selection of #AgsNotation.
 */

static gpointer ags_remove_region_from_selection_parent_class = NULL;
static AgsConnectableInterface *ags_remove_region_from_selection_parent_connectable_interface;

enum{
  PROP_0,
  PROP_NOTATION,
  PROP_X0,
  PROP_X1,
  PROP_Y0,
  PROP_Y1,
};

GType
ags_remove_region_from_selection_get_type()
{
  static GType ags_type_remove_region_from_selection = 0;

  if(!ags_type_remove_region_from_selection){
    static const GTypeInfo ags_remove_region_from_selection_info = {
      sizeof (AgsRemoveRegionFromSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_region_from_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveRegionFromSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_region_from_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_region_from_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_region_from_selection = g_type_register_static(AGS_TYPE_TASK,
								   "AgsRemoveRegionFromSelection\0",
								   &ags_remove_region_from_selection_info,
								   0);

    g_type_add_interface_static(ags_type_remove_region_from_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_region_from_selection);
}

void
ags_remove_region_from_selection_class_init(AgsRemoveRegionFromSelectionClass *remove_region_from_selection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_remove_region_from_selection_parent_class = g_type_class_peek_parent(remove_region_from_selection);

  /* gobject */
  gobject = (GObjectClass *) remove_region_from_selection;

  gobject->set_property = ags_remove_region_from_selection_set_property;
  gobject->get_property = ags_remove_region_from_selection_get_property;

  gobject->finalize = ags_remove_region_from_selection_finalize;

  /* properties */
  /**
   * AgsRemoveRegionFromSelection:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("notation\0",
				   "notation of remove note\0",
				   "The notation of remove note task\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);
  
  /**
   * AgsRemoveRegionFromSelection:x0:
   *
   * Note offset x0.
   * 
   * Since: 0.7.117
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
   * AgsRemoveRegionFromSelection:x1:
   *
   * Note offset x1.
   * 
   * Since: 0.7.117
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
   * AgsRemoveRegionFromSelection:y0:
   *
   * Note offset y0.
   * 
   * Since: 0.7.117
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
   * AgsRemoveRegionFromSelection:y1:
   *
   * Note offset y1.
   * 
   * Since: 0.7.117
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

  /* task */
  task = (AgsTaskClass *) remove_region_from_selection;

  task->launch = ags_remove_region_from_selection_launch;
}

void
ags_remove_region_from_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_region_from_selection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_region_from_selection_connect;
  connectable->disconnect = ags_remove_region_from_selection_disconnect;
}

void
ags_remove_region_from_selection_init(AgsRemoveRegionFromSelection *remove_region_from_selection)
{
  remove_region_from_selection->notation = NULL;
  remove_region_from_selection->x0 = 0;
  remove_region_from_selection->y0 = 0;
  remove_region_from_selection->x1 = 0;
  remove_region_from_selection->y1 = 0;
}

void
ags_remove_region_from_selection_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec)
{
  AgsRemoveRegionFromSelection *remove_region_from_selection;

  remove_region_from_selection = AGS_REMOVE_REGION_FROM_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(remove_region_from_selection->notation == (GObject *) notation){
	return;
      }

      if(remove_region_from_selection->notation != NULL){
	g_object_unref(remove_region_from_selection->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      remove_region_from_selection->notation = (GObject *) notation;
    }
    break;
  case PROP_X0:
    {
      remove_region_from_selection->x0 = g_value_get_uint(value);
    }
    break;
  case PROP_X1:
    {
      remove_region_from_selection->x1 = g_value_get_uint(value);
    }
    break;
  case PROP_Y0:
    {
      remove_region_from_selection->y0 = g_value_get_uint(value);
    }
    break;
  case PROP_Y1:
    {
      remove_region_from_selection->y1 = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_region_from_selection_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec)
{
  AgsRemoveRegionFromSelection *remove_region_from_selection;

  remove_region_from_selection = AGS_REMOVE_REGION_FROM_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, remove_region_from_selection->notation);
    }
    break;
  case PROP_X0:
    {
      g_value_set_uint(value, remove_region_from_selection->x0);
    }
    break;
  case PROP_X1:
    {
      g_value_set_uint(value, remove_region_from_selection->x1);
    }
    break;
  case PROP_Y0:
    {
      g_value_set_uint(value, remove_region_from_selection->y0);
    }
    break;
  case PROP_Y1:
    {
      g_value_set_uint(value, remove_region_from_selection->y1);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_region_from_selection_connect(AgsConnectable *connectable)
{
  ags_remove_region_from_selection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_region_from_selection_disconnect(AgsConnectable *connectable)
{
  ags_remove_region_from_selection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_region_from_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_region_from_selection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_region_from_selection_launch(AgsTask *task)
{
  AgsRemoveRegionFromSelection *remove_region_from_selection;

  remove_region_from_selection = AGS_REMOVE_REGION_FROM_SELECTION(task);

  /* remove region */
  ags_notation_remove_region_from_selection(remove_region_from_selection->notation,
					    remove_region_from_selection->x0, remove_region_from_selection->y0,
					    remove_region_from_selection->x1, remove_region_from_selection->y1);
}

/**
 * ags_remove_region_from_selection_new:
 * @notation: the #AgsNotation providing the selection
 * @x0: start x coordinate
 * @y0: start y coordinate
 * @x1: end x coordinate
 * @y1: end y coordinate
 *
 * Creates an #AgsRemoveRegionFromSelection.
 *
 * Returns: an new #AgsRemoveRegionFromSelection.
 *
 * Since: 0.4
 */
AgsRemoveRegionFromSelection*
ags_remove_region_from_selection_new(AgsNotation *notation,
				     guint x0, guint y0,
				     guint x1, guint y1)
{
  AgsRemoveRegionFromSelection *remove_region_from_selection;

  remove_region_from_selection = (AgsRemoveRegionFromSelection *) g_object_new(AGS_TYPE_REMOVE_REGION_FROM_SELECTION,
									       NULL);

  remove_region_from_selection->notation = notation;
  remove_region_from_selection->x0 = x0;
  remove_region_from_selection->y0 = y0;
  remove_region_from_selection->x1 = x1;
  remove_region_from_selection->y1 = y1;

  return(remove_region_from_selection);
}
