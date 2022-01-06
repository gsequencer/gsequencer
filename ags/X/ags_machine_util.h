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

#ifndef __AGS_MACHINE_UTIL_H__
#define __AGS_MACHINE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

GtkWidget* ags_machine_util_new_panel();
GtkWidget* ags_machine_util_new_mixer();
GtkWidget* ags_machine_util_new_spectrometer();
GtkWidget* ags_machine_util_new_equalizer();
GtkWidget* ags_machine_util_new_drum();
GtkWidget* ags_machine_util_new_matrix();
GtkWidget* ags_machine_util_new_synth();
GtkWidget* ags_machine_util_new_fm_synth();
GtkWidget* ags_machine_util_new_syncsynth();
GtkWidget* ags_machine_util_new_fm_syncsynth();
GtkWidget* ags_machine_util_new_hybrid_synth();
GtkWidget* ags_machine_util_new_fm_hybrid_synth();
GtkWidget* ags_machine_util_new_ffplayer();
GtkWidget* ags_machine_util_new_sf2_synth();
GtkWidget* ags_machine_util_new_pitch_sampler();
GtkWidget* ags_machine_util_new_sfz_synth();
GtkWidget* ags_machine_util_new_audiorec();
GtkWidget* ags_machine_util_new_desk();
GtkWidget* ags_machine_util_new_ladspa_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_dssi_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_vst3_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_lv2_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_live_dssi_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_live_lv2_bridge(gchar *filename, gchar *effect);
GtkWidget* ags_machine_util_new_live_vst3_bridge(gchar *filename, gchar *effect);

GtkWidget* ags_machine_util_new_by_type_name(gchar *machine_type_name,
					     gchar *filename, gchar *effect);

G_END_DECLS

#endif /*__AGS_MACHINE_UTIL_H__*/
