/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_preferences_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

gboolean
ags_preferences_delete_event_callback(GtkWidget *widget, GdkEventAny *event,
				      gpointer user_data)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  window->preferences = NULL;
  
  return(FALSE);
}

void
ags_preferences_response_callback(GtkDialog *dialog, gint response_id, gpointer user_data)
{
  gboolean apply;

  apply = FALSE;

  switch(response_id){
  case GTK_RESPONSE_APPLY:
    {
      apply = TRUE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(dialog));

      if(apply){
#if 0      
	ags_applicable_reset(AGS_APPLICABLE(dialog));
#endif
	
	break;
      }
    }
  case GTK_RESPONSE_CANCEL:
    {
      AgsWindow *window;

      AgsApplicationContext *application_context;

      application_context = ags_application_context_get_instance();
  
      window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

      AGS_PREFERENCES(dialog)->flags |= AGS_PREFERENCES_SHUTDOWN;

      window->preferences = NULL;
      
      gtk_widget_destroy(GTK_WIDGET(dialog));
    }
  }
}

void
ags_preferences_notebook_switch_page_callback(GtkNotebook *notebook,
					      gpointer page,
					      guint page_n,
					      AgsPreferences *preferences)
{
#if 0  
  GList *list, *list_start;

  list_start = 
    list = gtk_container_get_children((GtkContainer *) gtk_dialog_get_action_area(GTK_DIALOG(preferences)));
  list = g_list_nth(list,
		    3);
  
  while(list != NULL){
    gtk_widget_hide(list->data);

    list = list->next;
  }

  g_list_free(list_start);
  
  if(page_n == 1){
    gtk_widget_show((GtkWidget *) preferences->audio_preferences->add);
  }else if(page_n == 2){
    gtk_widget_show((GtkWidget *) preferences->midi_preferences->add);
  }
#endif
}
