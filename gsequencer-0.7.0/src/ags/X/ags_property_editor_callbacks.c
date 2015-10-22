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
