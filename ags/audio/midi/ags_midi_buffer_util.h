/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_MIDI_BUFFER_UTIL_H__
#define __AGS_MIDI_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <stdlib.h>

guint ags_midi_buffer_util_get_varlength_size(glong varlength);
void ags_midi_buffer_util_put_varlength(unsigned char *buffer,
					glong varlength);
guint ags_midi_buffer_util_get_varlength(unsigned char *buffer,
					 glong *varlength);

void ags_midi_buffer_util_put_key_on(unsigned char *buffer,
				     glong delta_time,
				     glong channel,
				     glong key,
				     glong velocity);

guint ags_midi_buffer_util_get_key_on(unsigned char *buffer,
				      glong *delta_time,
				      glong *channel,
				      glong *key,
				      glong *velocity);

void ags_midi_buffer_util_put_key_off(unsigned char *buffer,
				      glong delta_time,
				      glong channel,
				      glong key,
				      glong velocity);

guint ags_midi_buffer_util_get_key_off(unsigned char *buffer,
				       glong *delta_time,
				       glong *channel,
				       glong *key,
				       glong *velocity);

void ags_midi_buffer_util_put_key_pressure(unsigned char *buffer,
					   glong delta_time,
					   glong channel,
					   glong key,
					   glong pressure);

guint ags_midi_buffer_util_get_key_pressure(unsigned char *buffer,
					    glong *delta_time,
					    glong *channel,
					    glong *key,
					    glong *pressure);

#endif /*__AGS_MIDI_BUFFER_UTIL_H__*/
