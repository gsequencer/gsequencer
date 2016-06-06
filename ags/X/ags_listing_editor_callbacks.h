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

#ifndef __AGS_LISTING_EDITOR_CALLBACKS_H__
#define __AGS_LISTING_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_listing_editor.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

int ags_listing_editor_parent_set_callback(GtkWidget *widget,
					   GtkObject *old_parent,
					   AgsListingEditor *listing_editor);

void ags_listing_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
					  guint pads, guint pads_old,
					  AgsListingEditor *listing_editor);

#endif /*__AGS_LISTING_EDITOR_CALLBACKS_H__*/
