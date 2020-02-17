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

#include <ags/lib/ags_solver_term.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_solver_term_class_init(AgsSolverTermClass *solver_term);
void ags_solver_term_init (AgsSolverTerm *solver_term);
void ags_solver_term_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_solver_term_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_solver_term_finalize(GObject *gobject);

/**
 * SECTION:ags_solver_term
 * @short_description: solver term
 * @title: AgsSolverTerm
 * @section_id:
 * @include: ags/lib/ags_solver_term.h
 *
 * The #AgsSolverTerm contains the solver terms.
 */

enum{
  PROP_0,
};

static gpointer ags_solver_term_parent_class = NULL;

GType
ags_solver_term_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_term = 0;

    static const GTypeInfo ags_solver_term_info = {
      sizeof (AgsSolverTermClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_term_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverTerm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_term_init,
    };

    ags_type_solver_term = g_type_register_static(G_TYPE_OBJECT,
						  "AgsSolverTerm",
						  &ags_solver_term_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_term);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_term_class_init(AgsSolverTermClass *solver_term)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_term_parent_class = g_type_class_peek_parent(solver_term);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_term;

  gobject->set_property = ags_solver_term_set_property;
  gobject->get_property = ags_solver_term_get_property;
  
  gobject->finalize = ags_solver_term_finalize;

  /* properties */
}

void
ags_solver_term_init(AgsSolverTerm *solver_term)
{
  solver_term->flags = 0;
  
  g_rec_mutex_init(&(solver_term->obj_mutex));

  solver_term->term = NULL;

  solver_term->numeric_value = NULL;
}

void
ags_solver_term_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsSolverTerm *solver_term;

  solver_term = AGS_SOLVER_TERM(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_term_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsSolverTerm *solver_term;

  solver_term = AGS_SOLVER_TERM(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_term_finalize(GObject *gobject)
{
  AgsSolverTerm *solver_term;

  solver_term = AGS_SOLVER_TERM(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_solver_term_parent_class)->finalize(gobject);
}

/**
 * ags_solver_term_new:
 *
 * Instantiate a new #AgsSolverTerm.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsSolverTerm*
ags_solver_term_new()
{
  AgsSolverTerm *solver_term;
  
  solver_term = g_object_new(AGS_TYPE_SOLVER_TERM,
			     NULL);

  return(solver_term);
}
