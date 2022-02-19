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

#include <ags/app/ags_gsequencer_application_callbacks.h>

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
ags_gsequencer_export_callback(GAction *action, GVariant *parameter,
			       AgsGSequencerApplication *gsequencer_app)
{
  ags_app_action_util_export();
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
}

void
ags_gsequencer_add_vst3_bridge_callback(GAction *action, GVariant *parameter,
					AgsGSequencerApplication *gsequencer_app)
{
}
