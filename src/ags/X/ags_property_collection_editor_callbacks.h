/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__
#define __AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_collection_editor.h>

void ags_property_collection_editor_add_collection_callback(GtkButton *button,
							    AgsPropertyCollectionEditor *property_collection_editor);

void ags_property_collection_editor_remove_collection_callback(GtkButton *button,
							       GtkTable *table);

#endif /*__AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__*/
