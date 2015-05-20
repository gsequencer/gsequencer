/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_track_collection_callbacks.h>

void
ags_track_collection_enable_callback(GtkToggleButton *toggle,
				    AgsTrackCollection *track_collection)
{
  if(gtk_toggle_button_get_active(toggle)){
    track_collection->flags |= AGS_TRACK_COLLECTION_ENABLED;
  }else{
    track_collection->flags &= (~AGS_TRACK_COLLECTION_ENABLED);
  }
}
