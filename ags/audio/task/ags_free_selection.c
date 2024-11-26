/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/i18n.h>

void ags_free_selection_class_init(AgsFreeSelectionClass *free_selection);
void ags_free_selection_init(AgsFreeSelection *free_selection);
void ags_free_selection_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_free_selection_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_free_selection_dispose(GObject *gobject);
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

enum{
  PROP_0,
  PROP_NOTATION,
};

GType
ags_free_selection_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_free_selection = 0;

    static const GTypeInfo ags_free_selection_info = {
      sizeof(AgsFreeSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_free_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFreeSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_free_selection_init,
    };

    ags_type_free_selection = g_type_register_static(AGS_TYPE_TASK,
						     "AgsFreeSelection",
						     &ags_free_selection_info,
						     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_free_selection);
  }

  return(g_define_type_id__static);
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

  gobject->dispose = ags_free_selection_dispose;
  gobject->finalize = ags_free_selection_finalize;

  /* properties */
  /**
   * AgsFreeSelection:notation:
   *
   * The assigned #AgsNotation
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("notation of free selection"),
				   i18n_pspec("The notation of free selection task"),
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

      if(free_selection->notation == notation){
	return;
      }

      if(free_selection->notation != NULL){
	g_object_unref(free_selection->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      free_selection->notation = notation;
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
ags_free_selection_dispose(GObject *gobject)
{
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(gobject);

  if(free_selection->notation != NULL){
    g_object_unref(free_selection->notation);

    free_selection->notation = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_free_selection_parent_class)->dispose(gobject);
}

void
ags_free_selection_finalize(GObject *gobject)
{
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(gobject);

  if(free_selection->notation != NULL){
    g_object_unref(free_selection->notation);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_free_selection_parent_class)->finalize(gobject);
}

void
ags_free_selection_launch(AgsTask *task)
{
  AgsNotation *notation;
  
  AgsFreeSelection *free_selection;

  free_selection = AGS_FREE_SELECTION(task);

  notation = free_selection->notation;

  /* free selection */
  ags_notation_free_selection(notation);
}

/**
 * ags_free_selection_new:
 * @notation: the #AgsNotation
 *
 * Create a new instance of #AgsFreeSelection.
 *
 * Returns: the new #AgsFreeSelection.
 *
 * Since: 3.0.0
 */
AgsFreeSelection*
ags_free_selection_new(AgsNotation *notation)
{
  AgsFreeSelection *free_selection;

  free_selection = (AgsFreeSelection *) g_object_new(AGS_TYPE_FREE_SELECTION,
						     "notation", notation,
						     NULL);

  return(free_selection);
}
