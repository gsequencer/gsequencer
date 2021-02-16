/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2021 Joël Krähemann
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
#include <ags/libags-audio.h>

#include <stdlib.h>

int
main(int argc, char* argv[])
{
  guchar buffer[5];
  
  guint x;
  guint varlength_size;
  glong delta_time;

  if(argc == 2){
    x = g_ascii_strtoull(argv[1],
			 NULL,
			 10);
  }else{
    return(-1);
  }

  memset(buffer, 0, 5 * sizeof(guchar));
  
  delta_time = ags_midi_util_offset_to_delta_time(AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR,
						  0x60,
						  0x07A120,
						  AGS_SOUNDCARD_DEFAULT_BPM,
						  x);

  g_message("delta time = %d", delta_time);

  varlength_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  g_message("delta time (varlength) and size = 0x%x 0x%x 0x%x 0x%x 0x%x, %d",
	    buffer[0],
	    buffer[1],
	    buffer[2],
	    buffer[3],
	    buffer[4],
	    varlength_size);
  
  return(0);
}
