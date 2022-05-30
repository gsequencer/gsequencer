/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_ONLINE_HELP_WINDOW_CALLBACKS_H__
#define __AGS_ONLINE_HELP_WINDOW_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_online_help_window.h>

G_BEGIN_DECLS

#if defined(AGS_WITH_WEBKIT)
void ags_online_help_window_load_changed(WebKitWebView *web_view,
					 WebKitLoadEvent load_event,
					 AgsOnlineHelpWindow *online_help_window);
#endif

void ags_online_help_window_pdf_zoom_changed_callback(GtkComboBox *combo_box,
						      AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_pdf_drawing_area_draw_callback(GtkWidget *pdf_drawing_area,
							   cairo_t *cr,
							   int width, int height,
							   AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_pdf_drawing_area_resize_callback(GtkWidget *pdf_drawing_area,
							     gint width, gint height,
							     AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_pdf_vscrollbar_value_changed_callback(GtkAdjustment *vscrollbar,
								  AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_pdf_hscrollbar_value_changed_callback(GtkAdjustment *hscrollbar,
								  AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_home_callback(GtkWidget *button,
					  AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_next_callback(GtkWidget *button,
					  AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_prev_callback(GtkWidget *button,
					  AgsOnlineHelpWindow *online_help_window);

G_END_DECLS

#endif /*__AGS_ONLINE_HELP_WINDOW_CALLBACKS_H__*/
