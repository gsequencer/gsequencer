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

#include <ags/X/ags_property_editor_callbacks.h>

void
ags_property_editor_enable_callback(GtkToggleButton *toggle,
				    AgsPropertyEditor *property_editor)
{
  if(gtk_toggle_button_get_active(toggle)){
    property_editor->flags |= AGS_PROPERTY_EDITOR_ENABLED;
  }else{
    property_editor->flags &= (~AGS_PROPERTY_EDITOR_ENABLED);
  }
}
