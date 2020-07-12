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

#include <ags/X/ags_online_help_window_callbacks.h>

void
ags_online_help_window_load_changed(WebKitWebView *web_view,
				    WebKitLoadEvent load_event,
				    AgsOnlineHelpWindow *online_help_window)
{
  gchar *uri;

  uri = webkit_web_view_get_uri(web_view);

  gtk_entry_set_text(online_help_window->location,
		     uri);
}

void
ags_online_help_window_home_callback(GtkWidget *button,
				  AgsOnlineHelpWindow *online_help_window)
{

  webkit_web_view_load_uri(online_help_window->web_view,
			   online_help_window->start_filename);
}

void
ags_online_help_window_next_callback(GtkWidget *button,
				     AgsOnlineHelpWindow *online_help_window)
{
  webkit_web_view_go_forward(online_help_window->web_view);
}


void
ags_online_help_window_prev_callback(GtkWidget *button,
				     AgsOnlineHelpWindow *online_help_window)
{
  webkit_web_view_go_back(online_help_window->web_view);
}
