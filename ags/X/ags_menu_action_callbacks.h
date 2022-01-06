/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_MENU_ACTION_CALLBACKS_H__
#define __AGS_MENU_ACTION_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_MENU_ITEM_FILENAME_KEY "ags-menu-bar-filename-key"
#define AGS_MENU_ITEM_EFFECT_KEY "ags-menu-bar-effect-key"

void ags_menu_action_open_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_save_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_save_as_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_export_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_quit_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_panel_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_mixer_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_spectrometer_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_equalizer_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_drum_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_matrix_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_synth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_fm_synth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_syncsynth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_fm_syncsynth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_hybrid_synth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_fm_hybrid_synth_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_ffplayer_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_sf2_synth_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_pitch_sampler_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_sfz_synth_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_audiorec_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_desk_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_ladspa_bridge_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_dssi_bridge_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_lv2_bridge_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_vst3_bridge_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_add_live_dssi_bridge_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_live_lv2_bridge_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_add_live_vst3_bridge_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_notation_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_automation_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_wave_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_preferences_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_midi_import_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_midi_export_track_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_midi_playback_callback(GtkWidget *menu_item, gpointer data);

void ags_menu_action_online_help_callback(GtkWidget *menu_item, gpointer data);
void ags_menu_action_about_callback(GtkWidget *menu_item, gpointer data);

G_END_DECLS

#endif /*__AGS_MENU_ACTION_CALLBACKS_H__*/
