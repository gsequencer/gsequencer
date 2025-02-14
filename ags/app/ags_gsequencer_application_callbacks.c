/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_gsequencer_application_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/app/ags_app_action_util.h>

void
ags_gsequencer_open_callback(GAction *action, GVariant *parameter,
			     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_open();
}

void
ags_gsequencer_save_callback(GAction *action, GVariant *parameter,
			     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_save();
}

void
ags_gsequencer_save_as_callback(GAction *action, GVariant *parameter,
				AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_save_as();
}

void
ags_gsequencer_meta_data_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_meta_data();
}

void
ags_gsequencer_export_callback(GAction *action, GVariant *parameter,
			       AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_export();
}

void
ags_gsequencer_download_callback(GAction *action, GVariant *parameter,
			       AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_download();
}

void
ags_gsequencer_smf_import_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_smf_import();
}

void
ags_gsequencer_smf_export_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_smf_export();
}

void
ags_gsequencer_preferences_callback(GAction *action, GVariant *parameter,
				    AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_preferences();
}

void
ags_gsequencer_about_callback(GAction *action, GVariant *parameter,
			      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_about();
}

void
ags_gsequencer_help_callback(GAction *action, GVariant *parameter,
			     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_help();
}

void
ags_gsequencer_quit_callback(GAction *action, GVariant *parameter,
			     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_quit();
}

void
ags_gsequencer_present_app_window_callback(GAction *action, GVariant *parameter,
					   AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_preferences_window_callback(GAction *action, GVariant *parameter,
						   AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_meta_data_window_callback(GAction *action, GVariant *parameter,
						 AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_export_window_callback(GAction *action, GVariant *parameter,
					      AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_download_window_callback(GAction *action, GVariant *parameter,
					      AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_download_window(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_smf_import_window_callback(GAction *action, GVariant *parameter,
						  AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_midi_import_wizard(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_smf_export_window_callback(GAction *action, GVariant *parameter,
						  AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_midi_export_wizard(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_present_help_window_callback(GAction *action, GVariant *parameter,
					    AgsGSequencerApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
    
  gtk_window_present((GtkWindow *) ags_ui_provider_get_online_help_window(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_add_panel_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_panel();
}

void
ags_gsequencer_add_spectrometer_callback(GAction *action, GVariant *parameter,
					 AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_spectrometer();
}

void
ags_gsequencer_add_equalizer_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_equalizer();
}

void
ags_gsequencer_add_mixer_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_mixer();
}

void
ags_gsequencer_add_drum_callback(GAction *action, GVariant *parameter,
				 AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_drum();
}

void
ags_gsequencer_add_matrix_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_matrix();
}

void
ags_gsequencer_add_synth_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_synth();
}

void
ags_gsequencer_add_fm_synth_callback(GAction *action, GVariant *parameter,
				     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_fm_synth();
}

void
ags_gsequencer_add_syncsynth_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_syncsynth();
}

void
ags_gsequencer_add_fm_syncsynth_callback(GAction *action, GVariant *parameter,
					 AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_fm_syncsynth();
}

void
ags_gsequencer_add_hybrid_synth_callback(GAction *action, GVariant *parameter,
					 AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_hybrid_synth();
}

void
ags_gsequencer_add_hybrid_fm_synth_callback(GAction *action, GVariant *parameter,
					    AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_hybrid_fm_synth();
}

void
ags_gsequencer_add_stargazer_synth_callback(GAction *action, GVariant *parameter,
					    AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_stargazer_synth();
}

void
ags_gsequencer_add_quantum_synth_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_quantum_synth();
}

void
ags_gsequencer_add_ffplayer_callback(GAction *action, GVariant *parameter,
				     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_ffplayer();
}

void
ags_gsequencer_add_sf2_synth_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_sf2_synth();
}

void
ags_gsequencer_add_pitch_sampler_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_pitch_sampler();
}

void
ags_gsequencer_add_sfz_synth_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_sfz_synth();
}

void
ags_gsequencer_add_audiorec_callback(GAction *action, GVariant *parameter,
				     AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_audiorec();
}

void
ags_gsequencer_add_desk_callback(GAction *action, GVariant *parameter,
				 AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_add_desk();
}

void
ags_gsequencer_add_ladspa_bridge_callback(GAction *action, GVariant *parameter,
					  AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_ladspa_bridge(filename, effect);
}

void
ags_gsequencer_add_dssi_bridge_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_dssi_bridge(filename, effect);
}

void
ags_gsequencer_add_lv2_bridge_callback(GAction *action, GVariant *parameter,
				       AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_lv2_bridge(filename, effect);
}

void
ags_gsequencer_add_vst3_bridge_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_vst3_bridge(filename, effect);
}

void
ags_gsequencer_add_live_dssi_bridge_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_live_dssi_bridge(filename, effect);
}

void
ags_gsequencer_add_live_lv2_bridge_callback(GAction *action, GVariant *parameter,
					    AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_live_lv2_bridge(filename, effect);
}

void
ags_gsequencer_add_live_vst3_bridge_callback(GAction *action, GVariant *parameter,
					     AgsGSequencerApplication *gsequencer_app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  ags_app_action_util_add_live_vst3_bridge(filename, effect);
}

void
ags_gsequencer_edit_notation_callback(GAction *action, GVariant *parameter,
				      AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_notation();
}

void
ags_gsequencer_edit_automation_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_automation();
}

void
ags_gsequencer_edit_wave_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_wave();
}

void
ags_gsequencer_edit_sheet_callback(GAction *action, GVariant *parameter,
				   AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_sheet();
}

void
ags_gsequencer_edit_meta_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_meta();
}

void
ags_gsequencer_edit_tempo_callback(GAction *action, GVariant *parameter,
				  AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_edit_tempo();
}
