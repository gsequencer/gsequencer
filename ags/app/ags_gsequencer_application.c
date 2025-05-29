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

#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_callbacks.h>

#include "config.h"

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_gsequencer_resource.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/i18n.h>

void ags_gsequencer_application_class_init(AgsGSequencerApplicationClass *gsequencer_app);
void ags_gsequencer_application_init(AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_application_activate(GApplication *gsequencer_app);
void ags_gsequencer_application_startup(GApplication *gsequencer_app);
int ags_gsequencer_application_command_line(GApplication *gsequencer_app,
					    GApplicationCommandLine *command_line);
gboolean ags_gsequencer_application_local_command_line(GApplication *gsequencer_app,
						       gchar ***arguments,
						       int *exit_status);
void ags_gsequencer_application_open(GApplication *application,
				     GFile **files,
				     gint n_files,
				     const gchar *hint);

static gpointer ags_gsequencer_application_parent_class = NULL;

extern AgsApplicationContext *ags_application_context;

GType
ags_gsequencer_application_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_gsequencer_application = 0;

    static const GTypeInfo ags_gsequencer_application_info = {
      sizeof (AgsGSequencerApplicationClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gsequencer_application_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGSequencerApplication),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gsequencer_application_init,
    };

    ags_type_gsequencer_application = g_type_register_static(GTK_TYPE_APPLICATION,
							     "AgsGSequencerApplication",
							     &ags_gsequencer_application_info,
							     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_gsequencer_application);
  }

  return(g_define_type_id__static);
}

void
ags_gsequencer_application_class_init(AgsGSequencerApplicationClass *gsequencer_app)
{
  GApplicationClass *app;

  ags_gsequencer_application_parent_class = g_type_class_peek_parent(gsequencer_app);

  /* GApplicationClass */
  app = (GApplicationClass *) gsequencer_app;

  app->startup = ags_gsequencer_application_startup;
  app->activate = ags_gsequencer_application_activate;
  app->command_line = ags_gsequencer_application_command_line;
  app->local_command_line = ags_gsequencer_application_local_command_line;
  app->open = ags_gsequencer_application_open;
}

void
ags_gsequencer_application_init(AgsGSequencerApplication *gsequencer_app)
{
  GSimpleAction *open_action;
  GSimpleAction *save_action;
  GSimpleAction *save_as_action;
  GSimpleAction *meta_data_action;
  GSimpleAction *export_action;
  GSimpleAction *download_action;
  GSimpleAction *smf_import_action;
  GSimpleAction *smf_export_action;
  GSimpleAction *preferences_action;
  GSimpleAction *about_action;
  GSimpleAction *help_action;
  GSimpleAction *quit_action;

  GSimpleAction *present_app_window_action;
  GSimpleAction *present_preferences_window_action;
  GSimpleAction *present_meta_data_window_action;
  GSimpleAction *present_export_window_action;
  GSimpleAction *present_download_window_action;
  GSimpleAction *present_smf_import_window_action;
  GSimpleAction *present_smf_export_window_action;
  GSimpleAction *present_help_window_action;
  
  GSimpleAction *add_panel_action;
  GSimpleAction *add_spectrometer_action;
  GSimpleAction *add_equalizer_action;
  GSimpleAction *add_mixer_action;
  GSimpleAction *add_drum_action;
  GSimpleAction *add_matrix_action;
  GSimpleAction *add_synth_action;
  GSimpleAction *add_fm_synth_action;
  GSimpleAction *add_syncsynth_action;
  GSimpleAction *add_fm_syncsynth_action;
  GSimpleAction *add_hybrid_synth_action;
  GSimpleAction *add_hybrid_fm_synth_action;
  GSimpleAction *add_stargazer_synth_action;
  GSimpleAction *add_quantum_synth_action;
  GSimpleAction *add_raven_synth_action;
  GSimpleAction *add_ffplayer_action;
  GSimpleAction *add_sf2_synth_action;
  GSimpleAction *add_pitch_sampler_action;
  GSimpleAction *add_sfz_synth_action;
  GSimpleAction *add_audiorec_action;
  GSimpleAction *add_desk_action;

  GSimpleAction *add_ladspa_bridge_action;
  GSimpleAction *add_dssi_bridge_action;
  GSimpleAction *add_lv2_bridge_action;
  GSimpleAction *add_vst3_bridge_action;
  GSimpleAction *add_live_dssi_bridge_action;
  GSimpleAction *add_live_lv2_bridge_action;
  GSimpleAction *add_live_vst3_bridge_action;

  GSimpleAction *edit_notation_action;
  GSimpleAction *edit_automation_action;
  GSimpleAction *edit_wave_action;
  GSimpleAction *edit_sheet_action;
  GSimpleAction *edit_meta_action;
  GSimpleAction *edit_tempo_action;
  
  AgsApplicationContext *application_context;  

  FILE *paper_size_file;

  char buffer[256];
  gchar *paper_conf;
  gchar *paper_size;

#if defined(AGS_OSXAPI)
  const gchar * const open_accel[] = { "<Meta>o", NULL};
  const gchar * const save_accel[] = { "<Meta>s", NULL};
  const gchar * const save_as_accel[] = { "<Meta><Shift>s", NULL};
  const gchar * const help_accel[] = { "<Meta>h", NULL};
  const gchar * const quit_accel[] = { "<Meta>q", NULL};
#else
  const gchar * const open_accel[] = { "<Ctrl>o", NULL};
  const gchar * const save_accel[] = { "<Ctrl>s", NULL};
  const gchar * const save_as_accel[] = { "<Ctrl><Shift>s", NULL};
  const gchar * const help_accel[] = { "<Ctrl>h", NULL};
  const gchar * const quit_accel[] = { "<Ctrl>q", NULL};
#endif
  
  application_context = 
    ags_application_context = (AgsApplicationContext *) ags_gsequencer_application_context_new();
  g_object_ref(application_context);
  
  /* check /etc/papersize */
  paper_conf = g_getenv("PAPERCONF");
  paper_size = NULL;

  if(paper_conf != NULL){
    paper_conf = g_strdup(paper_conf);
  }
  
  if(paper_conf == NULL){
    paper_conf = g_strdup("/etc/papersize");
  }
  
  paper_size_file = fopen(paper_conf,
			  "r");

  memset(buffer, 0, 256 * sizeof(char));
  
  if(paper_size_file != NULL){
    char *paper_size_buffer;

    paper_size_buffer = fgets(buffer, 255, paper_size_file);
    buffer[255]= '\0';
    
    if(paper_size_buffer == NULL){
      buffer[0]= '\0';
      
      g_warning("fgets() returns NULL");
    }else{
      paper_size = NULL;
    }
    
    fclose(paper_size_file);
  }
  
  if(!g_strcmp0(buffer, "a3")){
    paper_size = "a4";
  }else if(!g_strcmp0(buffer, "b4")){
    paper_size = "a4";
  }else if(!g_strcmp0(buffer, "a4")){
    paper_size = "a4";
  }else if(!g_strcmp0(buffer, "b5")){
    paper_size = "a4";
  }else if(!g_strcmp0(buffer, "a5")){
    paper_size = "a4";
  }else if(!g_strcmp0(buffer, "tabloid")){
    paper_size = "letter";
  }else if(!g_strcmp0(buffer, "legal")){
    paper_size = "letter";
  }else if(!g_strcmp0(buffer, "letter")){
    paper_size = "letter";
  }else if(!g_strcmp0(buffer, "executive")){
    paper_size = "letter";
  }
  
  if(paper_size != NULL){
    g_free(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->paper_size);
    
    AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->paper_size = g_strdup(paper_size);
  }

  g_free(paper_conf);
  
  /* check PAPERSIZE environment variable */
  paper_size = g_getenv("PAPERSIZE");
  
  if(paper_size != NULL){
    g_free(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->paper_size);
    
    AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->paper_size = g_strdup(paper_size);
  }
  
  ags_ui_provider_set_app(AGS_UI_PROVIDER(application_context),
			  GTK_APPLICATION(gsequencer_app));
  
  /* open */
  open_action = g_simple_action_new("open",
				    NULL);
  g_signal_connect(open_action, "activate",
		   G_CALLBACK(ags_gsequencer_open_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(open_action));

  gtk_application_set_accels_for_action((GtkApplication *) gsequencer_app,
					"app.open",
					open_accel);
  
  /* save */
  save_action = g_simple_action_new("save",
				    NULL);
  g_signal_connect(save_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_action));

  gtk_application_set_accels_for_action((GtkApplication *) gsequencer_app,
					"app.save",
					save_accel);

  /* save as */
  save_as_action = g_simple_action_new("save_as",
				       NULL);
  g_signal_connect(save_as_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_as_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_as_action));

  gtk_application_set_accels_for_action((GtkApplication *) gsequencer_app,
					"app.save_as",
					save_as_accel);

  /* meta-data */
  meta_data_action = g_simple_action_new("meta_data",
					 NULL);
  g_signal_connect(meta_data_action, "activate",
		   G_CALLBACK(ags_gsequencer_meta_data_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(meta_data_action));

  /* export */
  export_action = g_simple_action_new("export",
				      NULL);
  g_signal_connect(export_action, "activate",
		   G_CALLBACK(ags_gsequencer_export_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(export_action));

  /* download */
#if defined(AGS_OSX_DMG_ENV) || defined(AGS_WITH_VTE)
  download_action = g_simple_action_new("download",
					NULL);
  g_signal_connect(download_action, "activate",
		   G_CALLBACK(ags_gsequencer_download_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(download_action));
#endif
  
  /* SMF import */
  smf_import_action = g_simple_action_new("smf_import",
					  NULL);
  g_signal_connect(smf_import_action, "activate",
		   G_CALLBACK(ags_gsequencer_smf_import_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(smf_import_action));

  /* SMF export */
  smf_export_action = g_simple_action_new("smf_export",
				      NULL);
  g_signal_connect(smf_export_action, "activate",
		   G_CALLBACK(ags_gsequencer_smf_export_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(smf_export_action));
  
  /* preferences */
  preferences_action = g_simple_action_new("preferences",
					   NULL);
  g_signal_connect(preferences_action, "activate",
		   G_CALLBACK(ags_gsequencer_preferences_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(preferences_action));

  /* about */
  about_action = g_simple_action_new("about",
				     NULL);
  g_signal_connect(about_action, "activate",
		   G_CALLBACK(ags_gsequencer_about_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(about_action));

  /* help */
  help_action = g_simple_action_new("help",
				    NULL);
  g_signal_connect(help_action, "activate",
		   G_CALLBACK(ags_gsequencer_help_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(help_action));

  gtk_application_set_accels_for_action((GtkApplication *) gsequencer_app,
					"app.help",
					help_accel);

  /* quit */
  quit_action = g_simple_action_new("quit",
				    NULL);
  g_signal_connect(quit_action, "activate",
		   G_CALLBACK(ags_gsequencer_quit_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(quit_action));

  gtk_application_set_accels_for_action((GtkApplication *) gsequencer_app,
					"app.quit",
					quit_accel);

  /* present app window */
  present_app_window_action = g_simple_action_new("present_app_window",
						  NULL);
  g_signal_connect(present_app_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_app_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_app_window_action));

  /* present preferences window */
  present_preferences_window_action = g_simple_action_new("present_preferences_window",
						  NULL);
  g_signal_connect(present_preferences_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_preferences_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_preferences_window_action));
  
  /* present meta-data window */
  present_meta_data_window_action = g_simple_action_new("present_meta_data_window",
							NULL);
  g_signal_connect(present_meta_data_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_meta_data_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_meta_data_window_action));

  /* present download window */
#if defined(AGS_WITH_VTE)
  present_download_window_action = g_simple_action_new("present_download_window",
						     NULL);
  g_signal_connect(present_download_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_download_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_download_window_action));
#endif
  
  /* present export window */
  present_export_window_action = g_simple_action_new("present_export_window",
						     NULL);
  g_signal_connect(present_export_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_export_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_export_window_action));

  /* present SMF import window */
  present_smf_import_window_action = g_simple_action_new("present_smf_import_window",
							 NULL);
  g_signal_connect(present_smf_import_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_smf_import_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_smf_import_window_action));

  /* present SMF export window */
  present_smf_export_window_action = g_simple_action_new("present_smf_export_window",
							 NULL);
  g_signal_connect(present_smf_export_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_smf_export_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_smf_export_window_action));

  /* present online help window */
  present_help_window_action = g_simple_action_new("present_help_window",
						   NULL);
  g_signal_connect(present_help_window_action, "activate",
		   G_CALLBACK(ags_gsequencer_present_help_window_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(present_help_window_action));
  
  /* panel */
  add_panel_action = g_simple_action_new("add_panel",
					 NULL);
  g_signal_connect(add_panel_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_panel_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_panel_action));

  /* panel */
  add_panel_action = g_simple_action_new("add_panel",
					 NULL);
  g_signal_connect(add_panel_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_panel_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_panel_action));
	
  /* spectrometer */
  add_spectrometer_action = g_simple_action_new("add_spectrometer",
						NULL);
  g_signal_connect(add_spectrometer_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_spectrometer_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_spectrometer_action));

  /* equalizer */
  add_equalizer_action = g_simple_action_new("add_equalizer",
					     NULL);
  g_signal_connect(add_equalizer_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_equalizer_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_equalizer_action));

  /* mixer */
  add_mixer_action = g_simple_action_new("add_mixer",
					 NULL);
  g_signal_connect(add_mixer_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_mixer_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_mixer_action));

  /* drum */
  add_drum_action = g_simple_action_new("add_drum",
					NULL);
  g_signal_connect(add_drum_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_drum_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_drum_action));

  /* matrix */
  add_matrix_action = g_simple_action_new("add_matrix",
					  NULL);
  g_signal_connect(add_matrix_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_matrix_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_matrix_action));

  /* synth */
  add_synth_action = g_simple_action_new("add_synth",
					 NULL);
  g_signal_connect(add_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_synth_action));

  /* FM synth */
  add_fm_synth_action = g_simple_action_new("add_fm_synth",
					    NULL);
  g_signal_connect(add_fm_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_fm_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_fm_synth_action));

  /* syncsynth */
  add_syncsynth_action = g_simple_action_new("add_syncsynth",
					     NULL);
  g_signal_connect(add_syncsynth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_syncsynth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_syncsynth_action));

  /* FM syncsynth */
  add_fm_syncsynth_action = g_simple_action_new("add_fm_syncsynth",
						NULL);
  g_signal_connect(add_fm_syncsynth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_fm_syncsynth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_fm_syncsynth_action));

  /* hybrid synth */
  add_hybrid_synth_action = g_simple_action_new("add_hybrid_synth",
						NULL);
  g_signal_connect(add_hybrid_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_hybrid_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_hybrid_synth_action));

  /* hybrid fm synth */
  add_hybrid_fm_synth_action = g_simple_action_new("add_hybrid_fm_synth",
						   NULL);
  g_signal_connect(add_hybrid_fm_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_hybrid_fm_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_hybrid_fm_synth_action));

  /* stargazer synth */
  add_stargazer_synth_action = g_simple_action_new("add_stargazer_synth",
						   NULL);
  g_signal_connect(add_stargazer_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_stargazer_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_stargazer_synth_action));

  /* quantum synth */
  add_quantum_synth_action = g_simple_action_new("add_quantum_synth",
						 NULL);
  g_signal_connect(add_quantum_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_quantum_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_quantum_synth_action));

  /* raven synth */
  add_raven_synth_action = g_simple_action_new("add_raven_synth",
						 NULL);
  g_signal_connect(add_raven_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_raven_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_raven_synth_action));

#if defined(AGS_WITH_LIBINSTPATCH)
  /* ffplayer */
  add_ffplayer_action = g_simple_action_new("add_ffplayer",
					    NULL);
  g_signal_connect(add_ffplayer_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_ffplayer_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_ffplayer_action));

  /* SF2 synth */
  add_sf2_synth_action = g_simple_action_new("add_sf2_synth",
					     NULL);
  g_signal_connect(add_sf2_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_sf2_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_sf2_synth_action));
#endif
  
  /* pitch sampler */
  add_pitch_sampler_action = g_simple_action_new("add_pitch_sampler",
						 NULL);
  g_signal_connect(add_pitch_sampler_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_pitch_sampler_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_pitch_sampler_action));

  /* SFZ synth */
  add_sfz_synth_action = g_simple_action_new("add_sfz_synth",
					     NULL);
  g_signal_connect(add_sfz_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_sfz_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_sfz_synth_action));

  /* audiorec */
  add_audiorec_action = g_simple_action_new("add_audiorec",
					    NULL);
  g_signal_connect(add_audiorec_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_audiorec_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_audiorec_action));  

#if 0
  /* desk */
  add_desk_action = g_simple_action_new("add_desk",
					NULL);
  g_signal_connect(add_desk_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_desk_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_desk_action));  
#endif
  
  /* LADSPA */
  add_ladspa_bridge_action = g_simple_action_new("add_ladspa_bridge",
						 G_VARIANT_TYPE("as"));
  g_signal_connect(add_ladspa_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_ladspa_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_ladspa_bridge_action));

  /* DSSI */
  add_dssi_bridge_action = g_simple_action_new("add_dssi_bridge",
					       G_VARIANT_TYPE("as"));
  g_signal_connect(add_dssi_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_dssi_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_dssi_bridge_action));

  /* LV2 */
  add_lv2_bridge_action = g_simple_action_new("add_lv2_bridge",
					      G_VARIANT_TYPE("as"));
  g_signal_connect(add_lv2_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_lv2_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_lv2_bridge_action));

  /* VST3 */
  add_vst3_bridge_action = g_simple_action_new("add_vst3_bridge",
					       G_VARIANT_TYPE("as"));
  g_signal_connect(add_vst3_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_vst3_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_vst3_bridge_action));

  /* live DSSI */
  add_live_dssi_bridge_action = g_simple_action_new("add_live_dssi_bridge",
						    G_VARIANT_TYPE("as"));
  g_signal_connect(add_live_dssi_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_live_dssi_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_live_dssi_bridge_action));

  /* live LV2 */
  add_live_lv2_bridge_action = g_simple_action_new("add_live_lv2_bridge",
						   G_VARIANT_TYPE("as"));
  g_signal_connect(add_live_lv2_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_live_lv2_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_live_lv2_bridge_action));

  /* live VST3 */
  add_live_vst3_bridge_action = g_simple_action_new("add_live_vst3_bridge",
						    G_VARIANT_TYPE("as"));
  g_signal_connect(add_live_vst3_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_live_vst3_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_live_vst3_bridge_action));

  /* edit notation */
  edit_notation_action = g_simple_action_new("edit_notation",
					     NULL);
  g_signal_connect(edit_notation_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_notation_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_notation_action));

  /* edit automation */
  edit_automation_action = g_simple_action_new("edit_automation",
					       NULL);
  g_signal_connect(edit_automation_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_automation_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_automation_action));

  /* edit wave */
  edit_wave_action = g_simple_action_new("edit_wave",
					 NULL);
  g_signal_connect(edit_wave_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_wave_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_wave_action));

  /* edit sheet */
  //NOTE:JK: uncomment
#if 0
  edit_sheet_action = g_simple_action_new("edit_sheet",
					  NULL);

  g_signal_connect(edit_sheet_action, "activate",
  		   G_CALLBACK(ags_gsequencer_edit_sheet_callback), gsequencer_app);

  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_sheet_action));
#endif

  /* edit meta */
  edit_meta_action = g_simple_action_new("edit_meta",
					 NULL);
  g_signal_connect(edit_meta_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_meta_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_meta_action));

  /* edit tempo */
  edit_tempo_action = g_simple_action_new("edit_tempo",
					 NULL);
  g_signal_connect(edit_tempo_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_tempo_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_tempo_action));
}

void
ags_gsequencer_application_startup(GApplication *app)
{
  GtkBuilder *builder;

  AgsGSequencerApplication *gsequencer_app;
  GMenu *menu;
  GMenuItem *item;
  
  AgsApplicationContext *application_context;
  AgsLog *log;

  gsequencer_app = (AgsGSequencerApplication *) app;

  G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->startup(app);

  /* application context */
  application_context = ags_application_context_get_instance();
    
  log = ags_log_get_instance();
  
  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer");

  gsequencer_app->menubar = (GMenu *) g_menu_new();
  
  /* file menu */
  gsequencer_app->file_menu = (GMenu *) g_menu_new();

  item = g_menu_item_new(i18n("Open"),
			 "app.open");
  g_menu_append_item(gsequencer_app->file_menu,
		     item);

  item = g_menu_item_new(i18n("Save"),
			 "app.save");
  g_menu_append_item(gsequencer_app->file_menu,
		     item);

  item = g_menu_item_new(i18n("Save as"),
			 "app.save_as");
  g_menu_append_item(gsequencer_app->file_menu,
		     item);
  
  item = g_menu_item_new(i18n("Export"),
			 "app.export");
  g_menu_append_item(gsequencer_app->file_menu,
		     item);
  
  item = g_menu_item_new(i18n("Quit"),
			 "app.quit");
  g_menu_append_item(gsequencer_app->file_menu,
		     item);

  g_menu_insert_submenu(gsequencer_app->menubar,
			-1,
			i18n("File"),
			G_MENU_MODEL(gsequencer_app->file_menu));
  
  /* edit menu */
  gsequencer_app->edit_menu = (GMenu *) g_menu_new();

  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_add_menu.ui");

  gsequencer_app->add_menu = (GMenu *) gtk_builder_get_object(builder,
							      "ags-add-menu");

  g_menu_insert_submenu(gsequencer_app->edit_menu,
			0,
			i18n("Add"),
			G_MENU_MODEL(gsequencer_app->add_menu));
  
  item = g_menu_item_new(i18n("Notation"),
			 "app.edit_notation");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  

  item = g_menu_item_new(i18n("Automation"),
			 "app.edit_automation");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  

  item = g_menu_item_new(i18n("Wave"),
			 "app.edit_wave");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  

  //NOTE:JK: uncomment
#if 0
  item = g_menu_item_new(i18n("Sheet"),
			 "app.edit_sheet");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  
#endif
  
  item = g_menu_item_new(i18n("Tempo"),
			 "app.edit_tempo");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  

  item = g_menu_item_new(i18n("Meta-data"),
			 "app.meta_data");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);

  item = g_menu_item_new(i18n("Preferences"),
			 "app.preferences");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  

#if defined(AGS_OSX_DMG_ENV)
  item = g_menu_item_new(i18n("Downloads"),
			 "app.download");
  g_menu_append_item(gsequencer_app->edit_menu,
		     item);  
#endif
  
  g_menu_insert_submenu(gsequencer_app->menubar,
			-1,
			i18n("Edit"),
			G_MENU_MODEL(gsequencer_app->edit_menu));

  /* MIDI menu */
  gsequencer_app->midi_menu = (GMenu *) g_menu_new();

  item = g_menu_item_new(i18n("SMF import"),
			 "app.smf_import");
  g_menu_append_item(gsequencer_app->midi_menu,
		     item);

  item = g_menu_item_new(i18n("SMF export"),
			 "app.smf_export");
  g_menu_append_item(gsequencer_app->midi_menu,
		     item);

  g_menu_insert_submenu(gsequencer_app->menubar,
			-1,
			i18n("MIDI"),
			G_MENU_MODEL(gsequencer_app->midi_menu));

  /* window menu */
  gsequencer_app->window_menu = NULL;
  
#if defined(AGS_OSX_DMG_ENV)
  gsequencer_app->window_menu = (GMenu *) g_menu_new();

  g_menu_insert_submenu(gsequencer_app->menubar,
			-1,
			i18n("Window"),
			G_MENU_MODEL(gsequencer_app->window_menu));
#endif
  
  /* Help menu */
  gsequencer_app->help_menu = (GMenu *) g_menu_new();

  item = g_menu_item_new(i18n("Help"),
			 "app.help");
  g_menu_append_item(gsequencer_app->help_menu,
		     item);

  item = g_menu_item_new(i18n("About"),
			 "app.about");
  g_menu_append_item(gsequencer_app->help_menu,
		     item);

  g_menu_insert_submenu(gsequencer_app->menubar,
			-1,
			i18n("Help"),
			G_MENU_MODEL(gsequencer_app->help_menu));

  /* initial refresh window menu */
  ags_gsequencer_application_refresh_window_menu(gsequencer_app);
}

void
ags_gsequencer_application_activate(GApplication *gsequencer_app)
{
  G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->activate(gsequencer_app);
}

int
ags_gsequencer_application_command_line(GApplication *gsequencer_app,
					GApplicationCommandLine *command_line)
{  
  G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->command_line(gsequencer_app,
									     command_line);
  
  g_message("command line");

  return(0);
}

gboolean
ags_gsequencer_application_local_command_line(GApplication *gsequencer_app,
					      gchar ***arguments,
					      int *exit_status)
{
  gchar *filename;

  gboolean handles_command_line;
  gboolean retval;
  
  retval = G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->local_command_line(gsequencer_app,
											    arguments,
											    exit_status);

  g_message("local command line");

  filename = NULL;

  handles_command_line = FALSE;

  if(arguments != NULL){
    gchar **iter;

    for(iter = arguments[0]; iter != NULL && iter[0] != NULL; iter++){
      if(!g_ascii_strncasecmp("--filename",
			      iter[0],
			      11)){
	handles_command_line = TRUE;
	
	iter++;
      }
    }
  }
  
  if(exit_status != NULL){
    exit_status[0] = 0;
  }

  return(TRUE);
}

void
ags_gsequencer_application_open(GApplication *application,
				GFile **files,
				gint n_files,
				const gchar *hint)
{
  AgsApplicationContext *application_context;

  /* application context */
  application_context = ags_application_context_get_instance();  
  
  if(n_files > 0 && files != NULL && files[0] != NULL){
    GtkWidget *window;
    GtkWidget *composite_editor;
    
    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

    if(composite_editor != NULL){
      GList *start_machine, *machine;
      GList *start_list, *list;

      ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor));
      
      /* destroy editor */
      g_menu_remove_all(AGS_COMPOSITE_EDITOR(composite_editor)->machine_selector->add_index_menu);

      AGS_COMPOSITE_EDITOR(composite_editor)->machine_selector->add_index_item_count = 0;      

      g_free(AGS_COMPOSITE_EDITOR(composite_editor)->machine_selector->add_index_item);
      
      AGS_COMPOSITE_EDITOR(composite_editor)->machine_selector->add_index_item = NULL;
      
      list =
	start_list = ags_machine_selector_get_machine_radio_button(AGS_COMPOSITE_EDITOR(composite_editor)->machine_selector);

      while(list != NULL){
	ags_machine_selector_remove_machine_radio_button(AGS_WINDOW(window)->composite_editor->machine_selector,
							 list->data);
    
	list = list->next;
      }

      g_list_free(start_list);

      machine = 
	start_machine = ags_ui_provider_get_machine(AGS_UI_PROVIDER(application_context));

      while(machine != NULL){
	AgsAudio *audio;
  
	AgsRemoveAudio *remove_audio;
	
	ags_machine_set_run(machine->data,
			    FALSE);
	
	/* destroy machine */
	audio = AGS_MACHINE(machine->data)->audio;
	g_object_ref(audio);

	ags_connectable_disconnect(AGS_CONNECTABLE(machine->data));
	ags_window_remove_machine((AgsWindow *) window,
				  (AgsMachine *) machine->data);

	/* get task thread */
	remove_audio = ags_remove_audio_new(audio);
	
	ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				      (AgsTask *) remove_audio);

	/* iterate */
	machine = machine->next;
      }
      
      g_list_free(start_machine);

      ags_ui_provider_set_machine(AGS_UI_PROVIDER(application_context),
				  NULL);

      AGS_COMPOSITE_EDITOR(composite_editor)->selected_machine = NULL;

      /* connect and and open filename */
      ags_connectable_connect(AGS_CONNECTABLE(composite_editor));

      if(files != NULL){
	AGS_WINDOW(window)->queued_filename = g_file_get_path(files[0]);
      }
    }
  }
}

/**
 * ags_gsequencer_application_refresh_window_menu:
 * @app: the #AgsGSequencerApplication
 *
 * Refresh window menu.
 *
 * Since: 6.6.0
 */
void
ags_gsequencer_application_refresh_window_menu(AgsGSequencerApplication *app)
{
  AgsApplicationContext *application_context;
  
  GMenuItem *item;

  GtkWindow *window;

#if defined(AGS_OSX_DMG_ENV)
  /* application context */
  application_context = ags_application_context_get_instance();

  g_menu_remove_all(app->window_menu);
  
  item = g_menu_item_new(i18n("application window"),
			 "app.present_app_window");
  g_menu_insert_item(app->window_menu,
		     -1,
		     item);

  if(ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("preferences"),
			   "app.present_preferences_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
  
#if defined(AGS_WITH_VTE)
  if(ags_ui_provider_get_download_window(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_download_window(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("sample downloads"),
			   "app.present_download_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
#endif

  if(ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("meta-data"),
			   "app.present_meta_data_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
  
  if(ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("audio export"),
			   "app.present_export_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
  
  if(ags_ui_provider_get_midi_import_wizard(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_midi_import_wizard(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("SMF import"),
			   "app.present_smf_import_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
  
  if(ags_ui_provider_get_midi_export_wizard(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_midi_export_wizard(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("SMF export"),
			   "app.present_smf_export_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
  
  if(ags_ui_provider_get_online_help_window(AGS_UI_PROVIDER(application_context)) != NULL &&
     gtk_widget_is_visible(ags_ui_provider_get_online_help_window(AGS_UI_PROVIDER(application_context)))){
    item = g_menu_item_new(i18n("help"),
			   "app.present_help_window");
    g_menu_insert_item(app->window_menu,
		       -1,
		       item);
  }
#endif
}

AgsGSequencerApplication*
ags_gsequencer_application_new(gchar *application_id,
			       GApplicationFlags flags)
{
  AgsGSequencerApplication *gsequencer_application;

  gsequencer_application = (AgsGSequencerApplication *) g_object_new(AGS_TYPE_GSEQUENCER_APPLICATION,
								     "application-id", application_id,
								     "flags", flags,
								     NULL);
 
  return(gsequencer_application);
}
