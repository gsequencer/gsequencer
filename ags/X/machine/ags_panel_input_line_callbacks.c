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

#include <ags/X/machine/ags_panel_input_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

void
ags_panel_input_line_notify_data_object_callback(GObject *audio_connection,
						 GParamSpec *pspec,
						 AgsPanelInputLine *panel_input_line)
{
  gchar *str;

  //FIXME:JK: uncomment if thread-safe
#if 0
  str = g_strdup_printf("%s:%s[%d]",
			G_OBJECT_TYPE_NAME(AGS_CONNECTION(audio_connection)->data_object),
			ags_soundcard_get_device(AGS_SOUNDCARD(AGS_CONNECTION(audio_connection)->data_object)),
			AGS_AUDIO_CONNECTION(audio_connection)->mapped_line + 1);
  gtk_label_set_label(panel_input_line->soundcard_connection,
		      str);

  g_free(str);
#endif
}

void
ags_panel_input_line_notify_mapped_line_callback(GObject *audio_connection,
						 GParamSpec *pspec,
						 AgsPanelInputLine *panel_input_line)
{
  gchar *str;
  
  //FIXME:JK: uncomment if thread-safe
#if 0
  str = g_strdup_printf("%s:%s[%d]",
			G_OBJECT_TYPE_NAME(AGS_CONNECTION(audio_connection)->data_object),
			ags_soundcard_get_device(AGS_SOUNDCARD(AGS_CONNECTION(audio_connection)->data_object)),
			AGS_AUDIO_CONNECTION(audio_connection)->mapped_line + 1);
  gtk_label_set_label(panel_input_line->soundcard_connection,
		      str);

  g_free(str);
#endif
}
