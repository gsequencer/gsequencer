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

#ifndef __AGS_SOUND_ENUMS_H__
#define __AGS_SOUND_ENUMS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_SOUND_ABILITY_FLAGS       (ags_sound_ability_flags_get_type())
#define AGS_TYPE_SOUND_BEHAVIOUR_FLAGS     (ags_sound_behaviour_flags_get_type())
#define AGS_TYPE_SOUND_SCOPE               (ags_sound_scope_get_type())
#define AGS_TYPE_SOUND_STAGING_FLAGS       (ags_sound_staging_flags_get_type())
#define AGS_TYPE_SOUND_STATE_FLAGS         (ags_sound_state_flags_get_type())
#define AGS_TYPE_SOUND_KEY_FORMAT          (ags_sound_key_format_get_type())

G_BEGIN_DECLS

/**
 * AgsSoundAbilityFlags: 
 * @AGS_SOUND_ABILITY_PLAYBACK: playback ability
 * @AGS_SOUND_ABILITY_SEQUENCER: sequenecer ability
 * @AGS_SOUND_ABILITY_NOTATION: notation abilitiy
 * @AGS_SOUND_ABILITY_WAVE: wave abilitiy
 * @AGS_SOUND_ABILITY_MIDI: MIDI ability
 * 
 * Enum values to specify particular ability. Mark an object to be
 * able to handle matching scope.
 */
typedef enum{
  AGS_SOUND_ABILITY_PLAYBACK            = 1,
  AGS_SOUND_ABILITY_SEQUENCER           = 1 <<  1,
  AGS_SOUND_ABILITY_NOTATION            = 1 <<  2,
  AGS_SOUND_ABILITY_WAVE                = 1 <<  3,
  AGS_SOUND_ABILITY_MIDI                = 1 <<  4,
}AgsSoundAbilityFlags;

/**
 * AgsSoundBehaviourFlags:
 * @AGS_SOUND_BEHAVIOUR_PATTERN_MODE: pattern mode
 * @AGS_SOUND_BEHAVIOUR_BULK_MODE: bulk mode
 * @AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING: reverse mapping
 * @AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT: defaults to output
 * @AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT: defaults to input
 * @AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: chained to output
 * @AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT: chained to input
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT: persistent
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK: persistent playback
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION: persistent notation
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER: persistent sequencer
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE: persistent wave
 * @AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI: persistent MIDI
 * @AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE: propagate done
 * 
 * Enum values to describe behaviour.
 */
typedef enum{
  AGS_SOUND_BEHAVIOUR_PATTERN_MODE           = 1,
  AGS_SOUND_BEHAVIOUR_BULK_MODE              = 1 <<  1,
  AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING        = 1 <<  2,
  AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT     = 1 <<  3,
  AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT      = 1 <<  4,
  AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT      = 1 <<  5,
  AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT       = 1 <<  6,
  AGS_SOUND_BEHAVIOUR_PERSISTENT             = 1 <<  7,
  AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK    = 1 <<  8,
  AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION    = 1 <<  9,
  AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER   = 1 << 10,
  AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE        = 1 << 11,
  AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI        = 1 << 12,
  AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE         = 1 << 13,
}AgsSoundBehaviourFlags;

/**
 * AgsSoundScope:
 * @AGS_SOUND_SCOPE_PLAYBACK: playback scope
 * @AGS_SOUND_SCOPE_SEQUENCER: sequencer scope
 * @AGS_SOUND_SCOPE_NOTATION: notation ability
 * @AGS_SOUND_SCOPE_WAVE: wave ability
 * @AGS_SOUND_SCOPE_MIDI: MIDI scope
 * @AGS_SOUND_SCOPE_LAST: the last scope
 * 
 * Enum values to specify current scope or request current scope.
 */
typedef enum{
  AGS_SOUND_SCOPE_PLAYBACK,
  AGS_SOUND_SCOPE_SEQUENCER,
  AGS_SOUND_SCOPE_NOTATION,
  AGS_SOUND_SCOPE_WAVE,
  AGS_SOUND_SCOPE_MIDI,
  AGS_SOUND_SCOPE_LAST,
}AgsSoundScope;

/**
 * AgsSoundStagingFlags:
 * @AGS_SOUND_STAGING_CHECK_RT_DATA: check rt-data
 * @AGS_SOUND_STAGING_RUN_INIT_PRE: run init pre
 * @AGS_SOUND_STAGING_RUN_INIT_INTER: run init inter
 * @AGS_SOUND_STAGING_RUN_INIT_POST: run init post
 * @AGS_SOUND_STAGING_FEED_INPUT_QUEUE: feed input queue
 * @AGS_SOUND_STAGING_AUTOMATE: automate
 * @AGS_SOUND_STAGING_RUN_PRE: run pre
 * @AGS_SOUND_STAGING_RUN_INTER: run inter
 * @AGS_SOUND_STAGING_RUN_POST: run post
 * @AGS_SOUND_STAGING_DO_FEEDBACK: do feedback
 * @AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE: feed output queue
 * @AGS_SOUND_STAGING_FINI: fini
 * @AGS_SOUND_STAGING_CANCEL: cancel
 * @AGS_SOUND_STAGING_DONE: done
 * @AGS_SOUND_STAGING_REMOVE: remove
 * @AGS_SOUND_STAGING_RESET: reset
 * @AGS_SOUND_STAGING_FX: fx staging
 * 
 * Enum values to stage the specified action.
 */
typedef enum{
  AGS_SOUND_STAGING_CHECK_RT_DATA       = 1,
  AGS_SOUND_STAGING_RUN_INIT_PRE        = 1 <<  1,
  AGS_SOUND_STAGING_RUN_INIT_INTER      = 1 <<  2,
  AGS_SOUND_STAGING_RUN_INIT_POST       = 1 <<  3,
  AGS_SOUND_STAGING_FEED_INPUT_QUEUE    = 1 <<  4,
  AGS_SOUND_STAGING_AUTOMATE            = 1 <<  5,
  AGS_SOUND_STAGING_RUN_PRE             = 1 <<  6,
  AGS_SOUND_STAGING_RUN_INTER           = 1 <<  7,
  AGS_SOUND_STAGING_RUN_POST            = 1 <<  8,
  AGS_SOUND_STAGING_DO_FEEDBACK         = 1 <<  9,
  AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE   = 1 << 10,
  AGS_SOUND_STAGING_FINI                = 1 << 11,
  AGS_SOUND_STAGING_CANCEL              = 1 << 12,
  AGS_SOUND_STAGING_DONE                = 1 << 13,
  AGS_SOUND_STAGING_REMOVE              = 1 << 14,
  AGS_SOUND_STAGING_RESET               = 1 << 15,
  AGS_SOUND_STAGING_FX                  = 1 << 16,
}AgsSoundStagingFlags;

/**
 * AgsSoundStateFlags:
 * @AGS_SOUND_STATE_IS_WAITING: is waiting
 * @AGS_SOUND_STATE_IS_ACTIVE: is active
 * @AGS_SOUND_STATE_IS_PROCESSING: is processing
 * @AGS_SOUND_STATE_IS_TERMINATING: is terminating
 * @AGS_SOUND_STATE_IS_PLUGIN_LOADED: plugin is loaded
 * @AGS_SOUND_STATE_IS_PORT_LOADED: port is loaded
 * 
 * Enum values to indicate state.
 */
typedef enum{
  AGS_SOUND_STATE_IS_WAITING                  = 1,
  AGS_SOUND_STATE_IS_ACTIVE                   = 1 <<  1,
  AGS_SOUND_STATE_IS_PROCESSING               = 1 <<  2,
  AGS_SOUND_STATE_IS_TERMINATING              = 1 <<  3,
  AGS_SOUND_STATE_IS_PLUGIN_LOADED            = 1 <<  4,
  AGS_SOUND_STATE_IS_PORT_LOADED              = 1 <<  5,
}AgsSoundStateFlags;

/**
 * AgsSoundKeyFormat:
 * @AGS_SOUND_KEY_FORMAT_16TH: key format 16th
 * @AGS_SOUND_KEY_FORMAT_256TH: key format 256th
 * 
 * Enum values to specify key format.
 */
typedef enum{
  AGS_SOUND_KEY_FORMAT_16TH,
  AGS_SOUND_KEY_FORMAT_256TH,
}AgsSoundKeyFormat;

GType ags_sound_ability_flags_get_type();
GType ags_sound_behaviour_flags_get_type();
GType ags_sound_scope_get_type();
GType ags_sound_staging_flags_get_type();
GType ags_sound_state_flags_get_type();
GType ags_sound_key_format_get_type();

G_END_DECLS

#endif /*__AGS_SOUND_ENUMS_H__*/
