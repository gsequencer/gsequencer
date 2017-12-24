/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_WAVE_TOOLBAR_CALLBACKS_H__
#define __AGS_WAVE_TOOLBAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/editor/ags_wave_toolbar.h>

void ags_wave_toolbar_tool_popup_select_audio_data_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);

#endif /*__AGS_WAVE_TOOLBAR_CALLBACKS_H__*/
