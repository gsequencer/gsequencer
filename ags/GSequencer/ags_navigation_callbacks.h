/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_NAVIGATION_CALLBACKS_H__
#define __AGS_NAVIGATION_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_navigation.h>

G_BEGIN_DECLS

void ags_navigation_expander_callback(GtkWidget *widget,
				      AgsNavigation *navigation);

void ags_navigation_bpm_callback(GtkWidget *widget,
				 AgsNavigation *navigation);

void ags_navigation_rewind_callback(GtkWidget *widget,
				    AgsNavigation *navigation);
void ags_navigation_prev_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_play_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_stop_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_next_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_forward_callback(GtkWidget *widget,
				     AgsNavigation *navigation);

void ags_navigation_loop_callback(GtkWidget *widget,
				  AgsNavigation *navigation);

void ags_navigation_position_tact_callback(GtkWidget *widget,
					   AgsNavigation *navigation);

void ags_navigation_duration_tact_callback(GtkWidget *widget,
					   AgsNavigation *navigation);

void ags_navigation_loop_left_tact_callback(GtkWidget *widget,
					    AgsNavigation *navigation);

void ags_navigation_loop_right_tact_callback(GtkWidget *widget,
					     AgsNavigation *navigation);

void ags_navigation_raster_callback(GtkWidget *widget,
				    AgsNavigation *navigation);

void ags_navigation_soundcard_stop_callback(GObject *soundcard,
					    AgsNavigation *navigation);

G_END_DECLS

#endif /*__AGS_NAVIGATION_CALLBACKS_H__*/
