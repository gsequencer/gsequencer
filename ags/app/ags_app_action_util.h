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

#ifndef __AGS_APP_ACTION_UTIL_H__
#define __AGS_APP_ACTION_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

void ags_app_action_util_open();
void ags_app_action_util_save();
void ags_app_action_util_save_as();

void ags_app_action_util_meta_data();

void ags_app_action_util_export();

void ags_app_action_util_smf_import();
void ags_app_action_util_smf_export();

void ags_app_action_util_preferences();

void ags_app_action_util_about();
void ags_app_action_util_help();

void ags_app_action_util_quit();

void ags_app_action_util_add_panel();
void ags_app_action_util_add_spectrometer();
void ags_app_action_util_add_equalizer();
void ags_app_action_util_add_mixer();
void ags_app_action_util_add_drum();
void ags_app_action_util_add_matrix();
void ags_app_action_util_add_synth();
void ags_app_action_util_add_fm_synth();
void ags_app_action_util_add_syncsynth();
void ags_app_action_util_add_fm_syncsynth();
void ags_app_action_util_add_hybrid_synth();
void ags_app_action_util_add_hybrid_fm_synth();
void ags_app_action_util_add_ffplayer();
void ags_app_action_util_add_sf2_synth();
void ags_app_action_util_add_pitch_sampler();
void ags_app_action_util_add_sfz_synth();
void ags_app_action_util_add_audiorec();
void ags_app_action_util_add_desk();

void ags_app_action_util_add_ladspa_bridge(gchar *filename, gchar *effect);
void ags_app_action_util_add_dssi_bridge(gchar *filename, gchar *effect);
void ags_app_action_util_add_lv2_bridge(gchar *filename, gchar *effect);
void ags_app_action_util_add_vst3_bridge(gchar *filename, gchar *effect);

void ags_app_action_util_add_live_dssi_bridge(gchar *filename, gchar *effect);
void ags_app_action_util_add_live_lv2_bridge(gchar *filename, gchar *effect);
void ags_app_action_util_add_live_vst3_bridge(gchar *filename, gchar *effect);

void ags_app_action_util_edit_notation();
void ags_app_action_util_edit_automation();
void ags_app_action_util_edit_wave();
void ags_app_action_util_edit_sheet();
void ags_app_action_util_edit_meta();
void ags_app_action_util_edit_tempo();

G_END_DECLS

#endif /*__AGS_APP_ACTION_UTIL_H__*/
