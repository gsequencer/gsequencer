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

#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_window.h>

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gsequencer_application);
  }

  return g_define_type_id__volatile;
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
  GSimpleAction *smf_import_action;
  GSimpleAction *smf_export_action;
  GSimpleAction *preferences_action;
  GSimpleAction *about_action;
  GSimpleAction *help_action;
  GSimpleAction *quit_action;

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
  
  AgsApplicationContext *application_context;  
  
  application_context = 
    ags_application_context = (AgsApplicationContext *) ags_gsequencer_application_context_new();
  g_object_ref(application_context);

  ags_ui_provider_set_app(AGS_UI_PROVIDER(application_context),
			  gsequencer_app);
      
  /* open */
  open_action = g_simple_action_new("open",
				    NULL);
  g_signal_connect(open_action, "activate",
		   G_CALLBACK(ags_gsequencer_open_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(open_action));

  gtk_application_set_accels_for_action(gsequencer_app,
					"app.open",
					"<Ctrl>o");
  
  /* save */
  save_action = g_simple_action_new("save",
				    NULL);
  g_signal_connect(save_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_action));

  gtk_application_set_accels_for_action(gsequencer_app,
					"app.save",
					"<Ctrl>s");

  /* save as */
  save_as_action = g_simple_action_new("save_as",
				       NULL);
  g_signal_connect(save_as_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_as_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_as_action));

  gtk_application_set_accels_for_action(gsequencer_app,
					"app.save_as",
					"<Ctrl><Shift>s");

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

  gtk_application_set_accels_for_action(gsequencer_app,
					"app.help",
					"<Ctrl>h");

  /* quit */
  quit_action = g_simple_action_new("quit",
				    NULL);
  g_signal_connect(quit_action, "activate",
		   G_CALLBACK(ags_gsequencer_quit_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(quit_action));

  gtk_application_set_accels_for_action(gsequencer_app,
					"app.quit",
					"<Ctrl>q");

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

  /* hybrid_synth */
  add_hybrid_synth_action = g_simple_action_new("add_hybrid_synth",
						NULL);
  g_signal_connect(add_hybrid_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_hybrid_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_hybrid_synth_action));

  /* hybrid_fm_synth */
  add_hybrid_fm_synth_action = g_simple_action_new("add_hybrid_fm_synth",
						   NULL);
  g_signal_connect(add_hybrid_fm_synth_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_hybrid_fm_synth_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_hybrid_fm_synth_action));

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
						 g_variant_type_new("as"));
  g_signal_connect(add_ladspa_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_ladspa_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_ladspa_bridge_action));

  /* DSSI */
  add_dssi_bridge_action = g_simple_action_new("add_dssi_bridge",
					       g_variant_type_new("as"));
  g_signal_connect(add_dssi_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_dssi_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_dssi_bridge_action));

  /* LV2 */
  add_lv2_bridge_action = g_simple_action_new("add_lv2_bridge",
					      g_variant_type_new("as"));
  g_signal_connect(add_lv2_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_lv2_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_lv2_bridge_action));

  /* VST3 */
  add_vst3_bridge_action = g_simple_action_new("add_vst3_bridge",
					       g_variant_type_new("as"));
  g_signal_connect(add_vst3_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_vst3_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_vst3_bridge_action));

  /* live DSSI */
  add_live_dssi_bridge_action = g_simple_action_new("add_live_dssi_bridge",
						    g_variant_type_new("as"));
  g_signal_connect(add_live_dssi_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_live_dssi_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_live_dssi_bridge_action));

  /* live LV2 */
  add_live_lv2_bridge_action = g_simple_action_new("add_live_lv2_bridge",
						   g_variant_type_new("as"));
  g_signal_connect(add_live_lv2_bridge_action, "activate",
		   G_CALLBACK(ags_gsequencer_add_live_lv2_bridge_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(add_live_lv2_bridge_action));

  /* live VST3 */
  add_live_vst3_bridge_action = g_simple_action_new("add_live_vst3_bridge",
						    g_variant_type_new("as"));
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
#if 0
  edit_sheet_action = g_simple_action_new("edit_sheet",
					  NULL);
  g_signal_connect(edit_sheet_action, "activate",
		   G_CALLBACK(ags_gsequencer_edit_sheet_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(edit_sheet_action));
#endif
}

void
ags_gsequencer_application_startup(GApplication *gsequencer_app)
{
  AgsApplicationContext *application_context;
  AgsLog *log;

  G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->startup(gsequencer_app);

  /* application context */
  application_context = ags_application_context_get_instance();
    
  log = ags_log_get_instance();
  
  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer");
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
  AgsApplicationContext *application_context;
  
  gchar *filename;

  gboolean handles_command_line;
  gboolean retval;
  
  retval = G_APPLICATION_CLASS(ags_gsequencer_application_parent_class)->local_command_line(gsequencer_app,
											    arguments,
											    exit_status);

  g_message("local command line");

  /* application context */
  application_context = ags_application_context_get_instance();  

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
  
    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    if(window != NULL){
      GList *start_machine, *machine;
      GList *start_list, *list;

      /* destroy editor */
      list =
	start_list = ags_machine_selector_get_machine_radio_button(AGS_WINDOW(window)->composite_editor->machine_selector);

      while(list != NULL){
	ags_machine_selector_remove_machine_radio_button(AGS_WINDOW(window)->composite_editor->machine_selector,
							 list->data);

	g_object_run_dispose(list->data);
	g_object_unref(list->data);
    
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
	gtk_widget_destroy((GtkWidget *) machine->data);

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
      
      AGS_WINDOW(window)->filename = g_file_get_path(files[0]);
    }
  }
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
