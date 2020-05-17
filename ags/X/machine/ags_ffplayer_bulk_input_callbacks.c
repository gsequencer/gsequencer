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

#include <ags/X/machine/ags_ffplayer_bulk_input_callbacks.h>

void
ags_ffplayer_bulk_input_resize_audio_channels_callback(AgsFFPlayerBulkInput *ffplayer_bulk_input,
						       guint audio_channels, guint audio_channels_old,
						       gpointer data)
{
  if(audio_channels > audio_channels_old){
    ags_ffplayer_bulk_input_input_map_recall(ffplayer_bulk_input,
					     audio_channels_old,
					     0);
  }
}

void
ags_ffplayer_bulk_input_resize_pads_callback(AgsFFPlayerBulkInput *ffplayer_bulk_input, GType channel_type,
					     guint pads, guint pads_old,
					     gpointer data)
{
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      ags_ffplayer_bulk_input_input_map_recall(ffplayer_bulk_input,
					       0,
					       pads_old);
    }
  }
}
