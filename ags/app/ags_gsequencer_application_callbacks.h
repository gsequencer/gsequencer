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

void ags_gsequencer_meta_data_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_export_callback(GAction *action, GVariant *parameter,
				    AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_download_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_smf_import_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_smf_export_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_preferences_callback(GAction *action, GVariant *parameter,
					 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_about_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_help_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_quit_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_app_window_callback(GAction *action, GVariant *parameter,
						AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_preferences_window_callback(GAction *action, GVariant *parameter,
							AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_meta_data_window_callback(GAction *action, GVariant *parameter,
						      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_export_window_callback(GAction *action, GVariant *parameter,
						   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_download_window_callback(GAction *action, GVariant *parameter,
						     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_smf_import_window_callback(GAction *action, GVariant *parameter,
						       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_smf_export_window_callback(GAction *action, GVariant *parameter,
						       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_present_help_window_callback(GAction *action, GVariant *parameter,
						 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_panel_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_spectrometer_callback(GAction *action, GVariant *parameter,
					      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_equalizer_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_mixer_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_drum_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_matrix_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_synth_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_fm_synth_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_syncsynth_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_fm_syncsynth_callback(GAction *action, GVariant *parameter,
					      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_hybrid_synth_callback(GAction *action, GVariant *parameter,
					      AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_hybrid_fm_synth_callback(GAction *action, GVariant *parameter,
						 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_stargazer_synth_callback(GAction *action, GVariant *parameter,
						 AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_quantum_synth_callback(GAction *action, GVariant *parameter,
					       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_raven_synth_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_ffplayer_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_sf2_synth_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_pitch_sampler_callback(GAction *action, GVariant *parameter,
					       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_sfz_synth_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_audiorec_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_add_desk_callback(GAction *action, GVariant *parameter,
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

void ags_gsequencer_edit_notation_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_edit_automation_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_edit_wave_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_edit_sheet_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_edit_meta_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_edit_tempo_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app);

G_END_DECLS

#endif /*__AGS_GSEQUENCER_APPLICATION_CALLBACKS_H__*/
