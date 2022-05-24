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

#include <ags/app/ags_osc_server_preferences_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

void
ags_osc_server_preferences_start_callback(GtkButton *button, AgsOscServerPreferences *osc_server_preferences)
{
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    if(!ags_osc_server_test_flags(list->data,
				  AGS_OSC_SERVER_STARTED)){
      ags_osc_server_start(list->data);
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_osc_server_preferences_stop_callback(GtkButton *button, AgsOscServerPreferences *osc_server_preferences)
{
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    if(ags_osc_server_test_flags(list->data,
				 AGS_OSC_SERVER_STARTED)){
      ags_osc_server_stop(list->data);
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_osc_server_preferences_any_address_callback(GtkButton *button, AgsOscServerPreferences *osc_server_preferences)
{
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }
  
  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
      ags_osc_server_set_flags(list->data,
			       AGS_OSC_SERVER_ANY_ADDRESS);
    }else{
      ags_osc_server_unset_flags(list->data,
				 AGS_OSC_SERVER_ANY_ADDRESS);
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* set address fields in-/sensitive */
  if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
    gtk_widget_set_sensitive((GtkWidget *) osc_server_preferences->ip4_address,
			     FALSE);

    gtk_widget_set_sensitive((GtkWidget *) osc_server_preferences->ip6_address,
			     FALSE);
  }else{
    gtk_widget_set_sensitive((GtkWidget *) osc_server_preferences->ip4_address,
			     TRUE);

    gtk_widget_set_sensitive((GtkWidget *) osc_server_preferences->ip6_address,
			     TRUE);
  }
}

void
ags_osc_server_preferences_enable_ip4_callback(GtkButton *button, AgsOscServerPreferences *osc_server_preferences)
{
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
      ags_osc_server_set_flags(list->data,
			       AGS_OSC_SERVER_INET4);
    }else{
      ags_osc_server_unset_flags(list->data,
				 AGS_OSC_SERVER_INET4);
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
}

void
ags_osc_server_preferences_ip4_address_callback(GtkEditable *editable, AgsOscServerPreferences *osc_server_preferences)
{
  GtkEntryBuffer *entry_buffer;
  
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  gchar *ip4;
  
  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(editable));

    ip4 = gtk_entry_buffer_get_text(entry_buffer);
    
    g_object_set(list->data,
		 "ip4", ip4,
		 NULL);
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
}

void
ags_osc_server_preferences_enable_ip6_callback(GtkButton *button, AgsOscServerPreferences *osc_server_preferences)
{
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
      ags_osc_server_set_flags(list->data,
			       AGS_OSC_SERVER_INET6);
    }else{
      ags_osc_server_unset_flags(list->data,
				 AGS_OSC_SERVER_INET6);
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
}

void
ags_osc_server_preferences_ip6_address_callback(GtkEditable *editable, AgsOscServerPreferences *osc_server_preferences)
{
  GtkEntryBuffer *entry_buffer;

  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  gchar *ip6;

  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(editable));

    ip6 = gtk_entry_buffer_get_text(entry_buffer);

    g_object_set(list->data,
		 "ip6", ip6,
		 NULL);
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
}
  
void
ags_osc_server_preferences_port_callback(GtkEditable *editable, AgsOscServerPreferences *osc_server_preferences)
{
  GtkEntryBuffer *entry_buffer;

  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  gchar *str;
  
  guint server_port;
  
  if((AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE & (osc_server_preferences->flags)) != 0){
    return;
  }
  application_context = ags_application_context_get_instance();
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(editable));

    server_port = AGS_OSC_SERVER_DEFAULT_SERVER_PORT;

    str = gtk_entry_buffer_get_text(entry_buffer);

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);
    }
  
    g_object_set(list->data,
		 "server-port", server_port,
		 NULL);
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
}
