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

#ifndef __AGS_MENU_BAR_CALLBACKS_H__
#define __AGS_MENU_BAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <ags/X/ags_menu_bar.h>

gboolean ags_menu_bar_destroy_callback(GtkObject *object, AgsMenuBar *menu_bar);
void ags_menu_bar_show_callback(GtkWidget *widget, AgsMenuBar *menu_bar);

void ags_menu_bar_open_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_save_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_save_as_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_export_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_import_midi_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_quit_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_panel_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_mixer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_drum_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_matrix_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_synth_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_ffplayer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_replicator_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_ladspa_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_lv2_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_remove_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_automation_editor_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_preferences_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_about_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

#endif /*__AGS_MENU_BAR_CALLBACKS_H__*/
