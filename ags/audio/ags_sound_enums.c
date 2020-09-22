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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>

GType
ags_sound_ability_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_SOUND_ABILITY_PLAYBACK, "AGS_SOUND_ABILITY_PLAYBACK", "sound-ability-playback" },
      { AGS_SOUND_ABILITY_SEQUENCER, "AGS_SOUND_ABILITY_SEQUENCER", "sound-ability-sequencer" },
      { AGS_SOUND_ABILITY_NOTATION, "AGS_SOUND_ABILITY_NOTATION", "sound-ability-notation" },
      { AGS_SOUND_ABILITY_WAVE, "AGS_SOUND_ABILITY_WAVE", "sound-ability-wave" },
      { AGS_SOUND_ABILITY_MIDI, "AGS_SOUND_ABILITY_MIDI", "sound-ability-midi" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsSoundAbilityFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}
