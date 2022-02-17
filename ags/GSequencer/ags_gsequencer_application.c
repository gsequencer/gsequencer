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

#include <ags/GSequencer/ags_gsequencer_application.h>

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
  //TODO:JK: implement me
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
