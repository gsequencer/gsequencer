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

void ags_gsequencer_application_class_init(AgsGSequencerApplicationClass *gsequencer_app);
void ags_gsequencer_application_init(AgsGSequencerApplication *gsequencer_app);

void ags_gsequencer_application_activate(GApplication *app);

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

  /* GApplicationClass */
  app = (GApplicationClass *) gsequencer_app;

  app->activate = ags_gsequencer_application_activate;
}

void
ags_gsequencer_application_init(AgsGSequencerApplication *gsequencer_app)
{
  GSimpleAction *open_action;
  GSimpleAction *save_action;
  GSimpleAction *save_as_action;
  GSimpleAction *export_action;
  GSimpleAction *preferences_action;
  GSimpleAction *about_action;
  GSimpleAction *help_action;
  GSimpleAction *quit_action;
  GSimpleAction *add_ladspa_bridge_action;
  GSimpleAction *add_dssi_bridge_action;
  GSimpleAction *add_lv2_bridge_action;
  GSimpleAction *add_vst3_bridge_action;

  /* open */
  open_action = g_simple_action_new("open",
				    NULL);
  g_signal_connect(open_action, "activate",
		   G_CALLBACK(ags_gsequencer_open_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(open_action));

  /* save */
  save_action = g_simple_action_new("save",
				    NULL);
  g_signal_connect(save_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_action));

  /* save as */
  save_as_action = g_simple_action_new("save_as",
				       NULL);
  g_signal_connect(save_as_action, "activate",
		   G_CALLBACK(ags_gsequencer_save_as_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(save_as_action));

  /* export */
  export_action = g_simple_action_new("export",
				      NULL);
  g_signal_connect(export_action, "activate",
		   G_CALLBACK(ags_gsequencer_export_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(export_action));

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

  /* quit */
  quit_action = g_simple_action_new("quit",
				    NULL);
  g_signal_connect(quit_action, "activate",
		   G_CALLBACK(ags_gsequencer_quit_callback), gsequencer_app);
  g_action_map_add_action(G_ACTION_MAP(gsequencer_app),
			  G_ACTION(quit_action));

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
}

void
ags_gsequencer_application_activate(GApplication *app)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  /* application context */  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);
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
