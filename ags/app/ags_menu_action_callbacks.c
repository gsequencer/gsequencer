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

#include <ags/app/ags_menu_action_callbacks.h>

#include <ags/app/ags_app_action_util.h>

void
ags_menu_action_open_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_open();
}

void
ags_menu_action_save_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_save();
}

void
ags_menu_action_save_as_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_save_as();
}

void
ags_menu_action_export_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_export();
}

void
ags_menu_action_quit_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_quit();
}

void
ags_menu_action_add_callback(GtkWidget *menu_item, gpointer data)
{
}

void
ags_menu_action_add_panel_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_panel();
}

void
ags_menu_action_add_mixer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_mixer();
}

void
ags_menu_action_add_spectrometer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_spectrometer();
}

void
ags_menu_action_add_equalizer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_equalizer();
}

void
ags_menu_action_add_drum_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_drum();
}

void
ags_menu_action_add_matrix_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_matrix();
}

void
ags_menu_action_add_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_synth();
}

void
ags_menu_action_add_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_fm_synth();
}

void
ags_menu_action_add_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_syncsynth();
}

void
ags_menu_action_add_fm_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_fm_syncsynth();
}

void
ags_menu_action_add_hybrid_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_hybrid_synth();
}

void
ags_menu_action_add_hybrid_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_hybrid_fm_synth();
}

void
ags_menu_action_add_ffplayer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_ffplayer();
}

void
ags_menu_action_add_sf2_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_sf2_synth();
}

void
ags_menu_action_add_pitch_sampler_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_pitch_sampler();
}

void
ags_menu_action_add_sfz_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_sfz_synth();
}

void
ags_menu_action_add_audiorec_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_audiorec();
}

void
ags_menu_action_add_desk_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_desk();
}

void
ags_menu_action_add_ladspa_bridge_callback(GtkWidget *menu_item, gpointer data)
{

  gchar *filename, *effect;
  
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_ladspa_bridge(filename, effect);
}

void
ags_menu_action_add_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);  

  ags_app_action_util_add_dssi_bridge(filename, effect);
}

void
ags_menu_action_add_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_lv2_bridge(filename, effect);
}

void
ags_menu_action_add_vst3_bridge_callback(GtkWidget *menu_item, gpointer data)
{
#if defined(AGS_WITH_VST3)
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_vst3_bridge(filename, effect);
#endif
}

void
ags_menu_action_add_live_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_dssi_bridge(filename, effect);
}

void
ags_menu_action_add_live_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_lv2_bridge(filename, effect);
}

void
ags_menu_action_add_live_vst3_bridge_callback(GtkWidget *menu_item, gpointer data)
{
#if defined(AGS_WITH_VST3)
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_vst3_bridge(filename, effect);
#endif
}

void
ags_menu_action_notation_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_edit_notation();
}

void
ags_menu_action_automation_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_edit_automation();
}

void
ags_menu_action_wave_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_edit_wave();
}

void
ags_menu_action_sheet_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_edit_sheet();
}

void
ags_menu_action_meta_data_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_meta_data();
}

void
ags_menu_action_preferences_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_preferences();
}

void
ags_menu_action_midi_import_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_smf_import();
}

void
ags_menu_action_midi_export_track_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_smf_export();
}

void
ags_menu_action_midi_playback_callback(GtkWidget *menu_item, gpointer data)
{
  //TODO:JK: implement me
}

void
ags_menu_action_online_help_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_help();
}

void
ags_menu_action_about_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_about();
}
