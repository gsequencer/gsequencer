/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

typedef enum{
  AGS_SOUND_SCOPE_PLAYBACK,
  AGS_SOUND_SCOPE_NOTATION,
  AGS_SOUND_SCOPE_SEQUENCER,
  AGS_SOUND_SCOPE_WAVE,
  AGS_SOUND_SCOPE_MIDI_INPUT,
  AGS_SOUND_SCOPE_MIDI_OUTPUT,
  AGS_SOUND_SCOPE_LAST,
}AgsSoundScope;

typedef enum{
  AGS_SOUND_STAGING_CHECK_RT_DATA       = 1,
  AGS_SOUND_STAGING_RUN_INIT_PRE        = 1 <<  1,
  AGS_SOUND_STAGING_RUN_INIT_INTER      = 1 <<  2,
  AGS_SOUND_STAGING_RUN_INIT_POST       = 1 <<  3,
  AGS_SOUND_STAGING_FEED_INPUT_QUEUE    = 1 <<  4,
  AGS_SOUND_STAGING_RUN_PRE             = 1 <<  5,
  AGS_SOUND_STAGING_RUN                 = 1 <<  6,
  AGS_SOUND_STAGING_RUN_POST            = 1 <<  7,
  AGS_SOUND_STAGING_DO_FEEDBACK         = 1 <<  8,
  AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE   = 1 <<  9,
  AGS_SOUND_STAGING_CANCEL              = 1 << 10,
  AGS_SOUND_STAGING_DONE                = 1 << 11,
  AGS_SOUND_STAGING_REMOVE              = 1 << 12,
}AgsSoundStagingFlags;

#endif /*__AGS_SOUND_ENUMS_H__*/
