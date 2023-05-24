/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/midi/ags_midi_ci_1_1_util.h>

gpointer ags_midi_ci_1_1_util_copy(gpointer ptr);
void ags_midi_ci_1_1_util_free(gpointer ptr);

/**
 * SECTION:ags_midi_ci_1_1_util
 * @short_description: MIDI util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ci_1_1_util.h
 *
 * Utility functions for MIDI CI version 1.1.
 */

GType
ags_midi_ci_1_1_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ci_1_1_util = 0;

    ags_type_midi_ci_1_1_util =
      g_boxed_type_register_static("AgsMidiCI_1_1_Util",
				   (GBoxedCopyFunc) ags_midi_ci_1_1_util_copy,
				   (GBoxedFreeFunc) ags_midi_ci_1_1_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ci_1_1_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_midi_ci_1_1_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsMidiCI_1_1_Util));
 
  return(retval);
}

void
ags_midi_ci_1_1_util_free(gpointer ptr)
{
  g_free(ptr);
}
