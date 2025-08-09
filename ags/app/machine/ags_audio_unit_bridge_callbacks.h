/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_BRIDGE_CALLBACKS_H__
#define __AGS_AUDIO_UNIT_BRIDGE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_audio_unit_bridge.h>

G_BEGIN_DECLS

void ags_audio_unit_bridge_show_audio_unit_ui_callback(GAction *action, GVariant *parameter,
						       AgsAudioUnitBridge *audio_unit_bridge);

void ags_audio_unit_bridge_dial_changed_callback(GtkWidget *dial, AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_scale_changed_callback(GtkWidget *scale, AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_button_clicked_callback(GtkWidget *button, AgsAudioUnitBridge *audio_unit_bridge);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_BRIDGE_CALLBACKS_H__*/
