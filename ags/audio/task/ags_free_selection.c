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

#include <ags/audio/task/ags_free_selection.h>

#include <ags/object/ags_connectable.h>

void ags_free_selection_class_init(AgsFreeSelectionClass *free_selection);
void ags_free_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_free_selection_init(AgsFreeSelection *free_selection);
void ags_free_selection_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_free_selection_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_free_selection_connect(AgsConnectable *connectable);
void ags_free_selection_disconnect(AgsConnectable *connectable);
void ags_free_selection_finalize(GObject *gobject);

void ags_free_selection_launch(AgsTask *task);

/**
 * SECTION:ags_free_selection
 * @short_description: free selection object
 * @title: AgsFreeSelection
 * @section_id:
 * @include: ags/audio/task/ags_free_selection.h
 *
 * The #AgsFreeSelection task frees selection of #AgsNotation.
 */

static gpointer ags_free_selection_parent_class = NULL;
static AgsConnectableInterface *ags_free_selection_parent_connectable_interface;

enum{
  PROP_0,
  PROP_NOTATION,
};

GType
ags_free_selection_get_type()
{
  static GType ags_type_free_selection = 0;

  if(!ags_type_free_selection){
    static const GTypeInfo ags_free_selection_info = {
      sizeof (AgsFreeSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_free_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFreeSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_free_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_free_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_free_selection = g_type_register_static(AGS_TYPE_TASK,
						     "AgsFreeSelection\0",
						     &ags_free_selection_info,
						     0);

    g_type_add_interface_static(ags_type_free_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_free_selection);
}

void
ags_free_selection_class_init(AgsFreeSelectionClass *free_selection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_free_selection_parent_class = g_type_class_peek_parent(free_selection);

  /* gobject */
  gobject = (GObjectClass *) free_selection;

  gobject->set_property = ags_free_selection_set_property;
  gobject->get_property = ags_free_selection_get_property;

  gobject->finalize = ags_free_selection_finalize;

  /* properties */
  /**
   * AgsFreeSelection:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("notation\0",
				   "notation of free selection\0",
				   "The notation of free selection task\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) free_selection;

  task->launch = ags_free_selection_launch;
}

void
ags_free_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_free_selection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_free_selection_connect;
  connectable->disconnect = ags_free_selection_disconnect;
}

void
ags_free_selection_init(AgsFreeSelection *free_selection)
{
  free_selection->notation = NULL;
}

void
ags_free_selection_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(free_selection->notation == (GObject *) notation){
	return;
      }

      if(free_selection->notation != NULL){
	g_object_unref(free_selection->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      free_selection->notation = (GObject *) notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_free_selection_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(gobject);

  switch(prop_id){
  case PROP_NOTATION:
    {
      g_value_set_object(value, free_selection->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_free_selection_connect(AgsConnectable *connectable)
{
  ags_free_selection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_free_selection_disconnect(AgsConnectable *connectable)
{
  ags_free_selection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_free_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_free_selection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_free_selection_launch(AgsTask *task)
{
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(task);

  /* free selection */
  ags_notation_free_selection(free_selection->notation);
}

/**
 * ags_free_selection_new:
 * @notation: the #AgsNotation
 *
 * Creates an #AgsFreeSelection.
 *
 * Returns: an new #AgsFreeSelection.
 *
 * Since: 0.4
 */
AgsFreeSelection*
ags_free_selection_new(AgsNotation *notation)
{
  AgsFreeSelection *free_selection;

  free_selection = (AgsFreeSelection *) g_object_new(AGS_TYPE_FREE_SELECTION,
						     NULL);

  free_selection->notation = notation;

  return(free_selection);
}
