/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_sequencer_util.h>

#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/core-audio/ags_core_audio_midiin.h>

/**
 * SECTION:ags_sequencer_util
 * @short_description: sequencer util
 * @title: AgsSequencerUtil
 * @section_id:
 * @include: ags/audio/ags_sequencer_util.h
 *
 * Sequencer utility functions.
 */

/**
 * ags_sequencer_util_get_obj_mutex:
 * @sequencer: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Get object mutex of @sequencer.
 * 
 * Returns: GRecMutex pointer
 * 
 * Since: 3.0.0
 */
GRecMutex*
ags_sequencer_util_get_obj_mutex(GObject *sequencer)
{
  GRecMutex *obj_mutex;

  obj_mutex = NULL;
  
  if(AGS_IS_MIDIIN(sequencer)){
    obj_mutex = AGS_MIDIIN_GET_OBJ_MUTEX(sequencer);
  }else if(AGS_IS_JACK_MIDIIN(sequencer)){
    obj_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(sequencer);
  }else if(AGS_IS_CORE_AUDIO_MIDIIN(sequencer)){
    obj_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(sequencer);
  }else{
    g_warning("unknown sequencer implementation");
  }

  return(obj_mutex);
}
