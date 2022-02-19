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

#ifndef __AGS_GSEQUENCER_APPLICATION_CALLBACKS_H__
#define __AGS_GSEQUENCER_APPLICATION_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_gsequencer_application.h>

G_BEGIN_DECLS

void ags_gsequencer_open_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_save_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_save_as_callback(GAction *action, GVariant *parameter,
				     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_export_callback(GAction *action, GVariant *parameter,
				    AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_preferences_callback(GAction *action, GVariant *parameter,
					 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_about_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_help_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_quit_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_ladspa_bridge_callback(GAction *action, GVariant *parameter,
					       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_dssi_bridge_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_lv2_bridge_callback(GAction *action, GVariant *parameter,
					    AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_vst3_bridge_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_live_dssi_bridge_callback(GAction *action, GVariant *parameter,
						  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_live_lv2_bridge_callback(GAction *action, GVariant *parameter,
						 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_live_vst3_bridge_callback(GAction *action, GVariant *parameter,
						  AgsGSequencerApplication *gsequencer_app);

G_END_DECLS

#endif /*__AGS_GSEQUENCER_APPLICATION_CALLBACKS_H__*/
