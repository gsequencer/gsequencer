/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine_counter_manager.h>

void ags_machine_counter_manager_class_init(AgsMachineCounterManagerClass *machine_counter_manager);
void ags_machine_counter_manager_init (AgsMachineCounterManager *machine_counter_manager);
void ags_machine_counter_manager_dispose(GObject *gobject);
void ags_machine_counter_manager_finalize(GObject *gobject);

static gpointer ags_machine_counter_manager_parent_class = NULL;

AgsMachineCounterManager *ags_machine_counter_manager = NULL;

GType
ags_machine_counter_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_counter_manager = 0;

    static const GTypeInfo ags_machine_counter_manager_info = {
      sizeof (AgsMachineCounterManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_counter_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineCounterManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_counter_manager_init,
    };

    ags_type_machine_counter_manager = g_type_register_static(G_TYPE_OBJECT,
							      "AgsMachineCounterManager",
							      &ags_machine_counter_manager_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_counter_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_counter_manager_class_init(AgsMachineCounterManagerClass *machine_counter_manager)
{
  GObjectClass *gobject;

  ags_machine_counter_manager_parent_class = g_type_class_peek_parent(machine_counter_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_counter_manager;

  gobject->dispose = ags_machine_counter_manager_dispose;
  gobject->finalize = ags_machine_counter_manager_finalize;
}

void
ags_machine_counter_manager_init(AgsMachineCounterManager *machine_counter_manager)
{
  machine_counter_manager->machine_counter = NULL;
}

void
ags_machine_counter_manager_dispose(GObject *gobject)
{
  AgsMachineCounterManager *machine_counter_manager;

  machine_counter_manager = AGS_MACHINE_COUNTER_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_machine_counter_manager_parent_class)->dispose(gobject);
}

void
ags_machine_counter_manager_finalize(GObject *gobject)
{
  AgsMachineCounterManager *machine_counter_manager;
  
  machine_counter_manager = AGS_MACHINE_COUNTER_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_machine_counter_manager_parent_class)->finalize(gobject);
}

/**
 * ags_machine_counter_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsMachineCounterManager
 *
 * Since: 4.0.0
 */
AgsMachineCounterManager*
ags_machine_counter_manager_get_instance()
{
  if(ags_machine_counter_manager == NULL){
    ags_machine_counter_manager = ags_machine_counter_manager_new();
  }

  return(ags_machine_counter_manager);
}

/**
 * ags_machine_counter_manager_new:
 *
 * Create a new instance of #AgsMachineCounterManager
 *
 * Returns: the new #AgsMachineCounterManager
 *
 * Since: 4.0.0
 */
AgsMachineCounterManager*
ags_machine_counter_manager_new()
{
  AgsMachineCounterManager *machine_counter_manager;

  machine_counter_manager = (AgsMachineCounterManager *) g_object_new(AGS_TYPE_MACHINE_COUNTER_MANAGER,
								      NULL);

  return(machine_counter_manager);
}
