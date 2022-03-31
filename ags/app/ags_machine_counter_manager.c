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

#include "config.h"

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>
#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_desk.h>
#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

#include <ags/i18n.h>

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
 * ags_machine_counter_manager_load:
 * @machine_counter_manager: the #AgsMachineCounterManager
 * 
 * Load default machine counters of @machine_counter_manager.
 *
 * Since: 4.0.0
 */
void
ags_machine_counter_manager_load(AgsMachineCounterManager *machine_counter_manager)
{
  GList *start_machine_counter;

  g_return_if_fail(AGS_IS_MACHINE_COUNTER_MANAGER(machine_counter_manager));

  start_machine_counter = NULL;

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_PANEL,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_MIXER,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_SPECTROMETER,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_EQUALIZER10,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_DRUM,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_MATRIX,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_FM_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_SYNCSYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_FM_SYNCSYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_HYBRID_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_HYBRID_FM_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_FFPLAYER,
								 NULL, NULL));

#if defined(AGS_WITH_LIBINSTPATCH)
  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_SF2_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_PITCH_SAMPLER,
								 NULL, NULL));
#endif
  
  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_SFZ_SYNTH,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_AUDIOREC,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_DESK,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_DESK,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_LADSPA_BRIDGE,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_DSSI_BRIDGE,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_LV2_BRIDGE,
								 NULL, NULL));

#if defined(AGS_WITH_VST3)
  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_VST3_BRIDGE,
								 NULL, NULL));
#endif
  
  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_LIVE_DSSI_BRIDGE,
								 NULL, NULL));

  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_LIVE_LV2_BRIDGE,
								 NULL, NULL));

#if defined(AGS_WITH_VST3)
  start_machine_counter = g_list_prepend(start_machine_counter,
					 ags_machine_counter_new(AGS_TYPE_LIVE_VST3_BRIDGE,
								 NULL, NULL));
#endif
  
  machine_counter_manager->machine_counter = start_machine_counter;
}

/**
 * ags_machine_counter_manager_find_machine_counter:
 * @machine_counter_manager: the #AgsMachineCounterManager
 * @machine_type: the #GType machine type to find
 * 
 * Find @machine_type of @machine_counter_manager.
 *
 * Returns: the matching #AgsMachineCounter or %NULL
 *
 * Since: 4.0.0
 */
AgsMachineCounter*
ags_machine_counter_manager_find_machine_counter(AgsMachineCounterManager *machine_counter_manager,
						 GType machine_type)
{
  AgsMachineCounter *machine_counter;
  
  GList *list;
  
  g_return_val_if_fail(AGS_IS_MACHINE_COUNTER_MANAGER(machine_counter_manager), NULL);

  machine_counter = NULL;
  
  list = machine_counter_manager->machine_counter;

  while(list != NULL){
    if(AGS_MACHINE_COUNTER(list->data)->machine_type == machine_type){
      machine_counter = list->data;
      
      break;
    }

    list = list->next;
  }

  return(machine_counter);  
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

    ags_machine_counter_manager_load(ags_machine_counter_manager);
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
