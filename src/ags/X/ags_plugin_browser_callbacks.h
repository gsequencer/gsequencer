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

#ifndef __AGS_PLUGIN_BROWSER_CALLBACKS_H__
#define __AGS_PLUGIN_BROWSER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_plugin_browser.h>

void ags_plugin_browser_ok_callback(GtkWidget *button,
				    AgsPluginBrowser *plugin_browser);
void ags_plugin_browser_cancel_callback(GtkWidget *button,
					AgsPluginBrowser *plugin_browser);

#endif /*__AGS_PLUGIN_BROWSER_CALLBACKS_H__*/
