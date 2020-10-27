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

GType
ags_sound_behaviour_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_SOUND_BEHAVIOUR_PATTERN_MODE, "AGS_SOUND_BEHAVIOUR_PATTERN_MODE", "sound-behaviour-pattern-mode" },
      { AGS_SOUND_BEHAVIOUR_BULK_MODE, "AGS_SOUND_BEHAVIOUR_BULK_MODE", "sound-behaviour-bulk-mode" },
      { AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING, "AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING", "sound-behaviour-reverse-mapping" },
      { AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT, "AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT", "sound-behaviour-defaults-to-output" },
      { AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT, "AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT", "sound-behaviour-defaults-to-input" },
      { AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT, "AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT", "sound-behaviour-chained-to-output" },
      { AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT, "AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT", "sound-behaviour-chained-to-input" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT, "AGS_SOUND_BEHAVIOUR_PERSISTENT", "sound-behaviour-persistent" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK, "AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK", "sound-behaviour-persistent-playback" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION, "AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION", "sound-behaviour-persistent-notation" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER, "AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER", "sound-behaviour-persistent-sequencer" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE, "AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE", "sound-behaviour-persistent-wave" },
      { AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI, "AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI", "sound-behaviour-persistent-midi" },
      { AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE, "AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE", "sound-behaviour-propagate-done" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsSoundBehaviourFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_sound_scope_get_type()
{
  static volatile gsize g_enum_type_id__volatile;

  if(g_once_init_enter (&g_enum_type_id__volatile)){
    static const GEnumValue values[] = {
      { AGS_SOUND_SCOPE_PLAYBACK, "AGS_SOUND_SCOPE_PLAYBACK", "sound-scope-playback" },
      { AGS_SOUND_SCOPE_SEQUENCER, "AGS_SOUND_SCOPE_SEQUENCER", "sound-scope-sequencer" },
      { AGS_SOUND_SCOPE_NOTATION, "AGS_SOUND_SCOPE_NOTATION", "sound-scope-notation" },
      { AGS_SOUND_SCOPE_WAVE, "AGS_SOUND_SCOPE_WAVE", "sound-scope-wave" },
      { AGS_SOUND_SCOPE_MIDI, "AGS_SOUND_SCOPE_MIDI", "sound-scope-midi" },
      { AGS_SOUND_SCOPE_LAST, "AGS_SOUND_SCOPE_LAST", "sound-scope-last" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSoundScope"), values);

    g_once_init_leave (&g_enum_type_id__volatile, g_enum_type_id);
  }
  
  return g_enum_type_id__volatile;
}

GType
ags_sound_staging_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_SOUND_STAGING_CHECK_RT_DATA, "AGS_SOUND_STAGING_CHECK_RT_DATA", "sound-staging-check-rt-data" },
      { AGS_SOUND_STAGING_RUN_INIT_PRE, "AGS_SOUND_STAGING_RUN_INIT_PRE", "sound-staging-run-init-pre" },
      { AGS_SOUND_STAGING_RUN_INIT_INTER, "AGS_SOUND_STAGING_RUN_INIT_INTER", "sound-staging-run-init-inter" },
      { AGS_SOUND_STAGING_RUN_INIT_POST, "AGS_SOUND_STAGING_RUN_INIT_POST", "sound-staging-run-init-post" },
      { AGS_SOUND_STAGING_FEED_INPUT_QUEUE, "AGS_SOUND_STAGING_FEED_INPUT_QUEUE", "sound-staging-feed-input-queue" },
      { AGS_SOUND_STAGING_AUTOMATE, "AGS_SOUND_STAGING_AUTOMATE", "sound-staging-automate" },
      { AGS_SOUND_STAGING_RUN_PRE, "AGS_SOUND_STAGING_RUN_PRE", "sound-staging-run-pre" },
      { AGS_SOUND_STAGING_RUN_INTER, "AGS_SOUND_STAGING_RUN_INTER", "sound-staging-run-inter" },
      { AGS_SOUND_STAGING_RUN_POST, "AGS_SOUND_STAGING_RUN_POST", "sound-staging-run-post" },
      { AGS_SOUND_STAGING_DO_FEEDBACK, "AGS_SOUND_STAGING_DO_FEEDBACK", "sound-staging-do-feedback" },
      { AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE, "AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE", "sound-staging-feed-output-queue" },
      { AGS_SOUND_STAGING_FINI, "AGS_SOUND_STAGING_FINI", "sound-staging-fini" },
      { AGS_SOUND_STAGING_CANCEL, "AGS_SOUND_STAGING_CANCEL", "sound-staging-cancel" },
      { AGS_SOUND_STAGING_DONE, "AGS_SOUND_STAGING_DONE", "sound-staging-done" },
      { AGS_SOUND_STAGING_REMOVE, "AGS_SOUND_STAGING_REMOVE", "sound-staging-remove" },
      { AGS_SOUND_STAGING_RESET, "AGS_SOUND_STAGING_RESET", "sound-staging-reset" },
      { AGS_SOUND_STAGING_FX, "AGS_SOUND_STAGING_FX", "sound-staging-fx" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsSoundStagingFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_sound_state_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_SOUND_STATE_IS_WAITING, "AGS_SOUND_STATE_IS-WAITING", "sound-state-is-waiting" },
      { AGS_SOUND_STATE_IS_ACTIVE, "AGS_SOUND_STATE_IS_ACTIVE", "sound-state-is-active" },
      { AGS_SOUND_STATE_IS_PROCESSING, "AGS_SOUND_STATE_IS_PROCESSING", "sound-state-is-processing" },
      { AGS_SOUND_STATE_IS_TERMINATING, "AGS_SOUND_STATE_IS_TERMINATING", "sound-state-is-terminating" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsSoundStateFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_sound_key_format_get_type()
{
  static volatile gsize g_enum_type_id__volatile;

  if(g_once_init_enter (&g_enum_type_id__volatile)){
    static const GEnumValue values[] = {
      { AGS_SOUND_KEY_FORMAT_16TH, "AGS_SOUND_KEY_FORMAT_16TH", "sound-key-format-16th" },
      { AGS_SOUND_KEY_FORMAT_256TH, "AGS_SOUND_KEY_FORMAT_256TH", "sound-key-format-256th" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSoundKeyFormat"), values);

    g_once_init_leave (&g_enum_type_id__volatile, g_enum_type_id);
  }
  
  return g_enum_type_id__volatile;
}
