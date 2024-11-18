/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_window_callbacks.h>

#include "config.h"

#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_app_action_util.h>

#if defined(AGS_OSX_DMG_ENV)
#include <AVFoundation/AVFormat.h>
#include <AVFoundation/AVFoundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#endif

#include <ags/i18n.h>

void
ags_window_setup_completed_callback(AgsApplicationContext *application_context, AgsWindow *window)
{
  GtkBuilder *builder;

  AgsGSequencerApplication *gsequencer_app;

  GMenu *menu;

  gsequencer_app = (AgsGSequencerApplication *) AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->app;

  /* menu */
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_add_menu.ui");

  menu = (GMenu *) gtk_builder_get_object(builder,
					  "ags-add-menu");

  gtk_menu_button_set_menu_model(window->add_button,
				 G_MENU_MODEL(menu));

  ags_window_load_add_menu_ladspa(window,
				  menu);
  ags_window_load_add_menu_ladspa(window,
				  gsequencer_app->add_menu);

  ags_window_load_add_menu_dssi(window,
				menu);
  ags_window_load_add_menu_dssi(window,
				gsequencer_app->add_menu);

  ags_window_load_add_menu_lv2(window,
			       menu);
  ags_window_load_add_menu_lv2(window,
			       gsequencer_app->add_menu);

  ags_window_load_add_menu_vst3(window,
				menu);
  ags_window_load_add_menu_vst3(window,
				gsequencer_app->add_menu);

  ags_window_load_add_menu_live_dssi(window,
				     menu);
  ags_window_load_add_menu_live_dssi(window,
				     gsequencer_app->add_menu);

  ags_window_load_add_menu_live_lv2(window,
				    menu);
  ags_window_load_add_menu_live_lv2(window,
				    gsequencer_app->add_menu);

  ags_window_load_add_menu_live_vst3(window,
				     menu);
  ags_window_load_add_menu_live_vst3(window,
				     gsequencer_app->add_menu);
  
  /* connect and show window */
  gtk_widget_show((GtkWidget *) window);

#if defined(AGS_OSX_DMG_ENV)
  // Request permission to access the camera and microphone.
  switch ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio])
    {
    case AVAuthorizationStatusAuthorized:
      {
        // The user has previously granted access to the microphone.
        break;
      }
    case AVAuthorizationStatusNotDetermined:
      {
        // The app hasn't yet asked the user for microphone access.
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
	    if(granted){
	      g_message("microphone access granted");
	    }else{
	      g_message("microphone access not granted");
	    }
	  }];
        break;
      }
    case AVAuthorizationStatusDenied:
      {
        // The user has previously denied access.
	g_message("microphone denied");

	break;
      }
    case AVAuthorizationStatusRestricted:
      {
        // The user can't grant access due to restrictions.
	g_message("microphone disabled");

	break;
      }
    }
#endif
}

gboolean
ags_window_close_request_callback(GtkWindow *window, gpointer user_data)
{
  ags_app_action_util_quit();
  
  return(TRUE);
}
