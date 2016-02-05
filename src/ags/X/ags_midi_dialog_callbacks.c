/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_midi_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/X/ags_window.h>

int
ags_midi_dialog_backend_changed_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(widget));
  
  if(!g_ascii_strncasecmp("alsa\0",
			  str,
			  4)){
    gtk_widget_set_sensitive(midi_dialog->jack_server,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->server_name,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->add_server,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->remove_server,
			     FALSE);

    gtk_widget_set_sensitive(midi_dialog->jack_client,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->client_name,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->add_client,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->remove_client,
			     FALSE);

    gtk_widget_set_sensitive(midi_dialog->port_name,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->add_port,
			     FALSE);
    gtk_widget_set_sensitive(midi_dialog->remove_port,
			     FALSE);
  }else if(!g_ascii_strncasecmp("jack\0",
				str,
				4)){
    gtk_widget_set_sensitive(midi_dialog->jack_server,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->server_name,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->add_server,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->remove_server,
			     TRUE);

    gtk_widget_set_sensitive(midi_dialog->jack_client,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->client_name,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->add_client,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->remove_client,
			     TRUE);

    gtk_widget_set_sensitive(midi_dialog->port_name,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->add_port,
			     TRUE);
    gtk_widget_set_sensitive(midi_dialog->remove_port,
			     TRUE);
  }

  ags_midi_dialog_load_sequencers(midi_dialog);

  return(0);
}

int
ags_midi_dialog_add_server_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(str == NULL ||
     g_utf8_strlen(str,
		   -1) == 0){
    return(0);
  }
  
  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    AgsWindow *window;
    AgsMachine *machine;

    AgsJackServer *jack_server;
    
    AgsMutexManager *mutex_manager;

    AgsApplicationContext *application_context;

    GList *list;
    
    gchar *server;

    pthread_mutex_t *application_mutex;
    
    machine = midi_dialog->machine;

    window = gtk_widget_get_ancestor(machine,
				     AGS_TYPE_WINDOW);

    /* application context and mutex manager */
    application_context = (AgsApplicationContext *) window->application_context;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* add server */
    jack_server = ags_jack_server_new(application_context,
				      server);
    
    pthread_mutex_lock(application_mutex);

    list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

    list = g_list_prepend(list,
			  jack_server);
    ags_sound_provider_set_distributed_manager(AGS_SOUND_PROVIDER(application_context),
					       list);
    
    pthread_mutex_unlock(application_mutex);
    
    /* fill combo box */    
    server = gtk_entry_get_text(midi_dialog->server_name);

    gtk_combo_box_text_append_text(midi_dialog->jack_server,
				   server);
  }
  
  return(0);
}

int
ags_midi_dialog_remove_server_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    gtk_combo_box_text_remove(midi_dialog->jack_server,
			      gtk_combo_box_get_active(midi_dialog->jack_server));
  }
  
  return(0);
}

int
ags_midi_dialog_add_client_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(str == NULL ||
     g_utf8_strlen(str,
		   -1) == 0){
    return(0);
  }
  
  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    AgsWindow *window;
    AgsMachine *machine;

    AgsJackServer *jack_server;
    AgsJackClient *jack_client;
    
    AgsMutexManager *mutex_manager;

    AgsApplicationContext *application_context;

    GList *list;

    gchar *server;
    gchar *client;

    pthread_mutex_t *application_mutex;
    
    machine = midi_dialog->machine;

    window = gtk_widget_get_ancestor(machine,
				     AGS_TYPE_WINDOW);

    /* application context and mutex manager */
    application_context = (AgsApplicationContext *) window->application_context;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* find server */
    server = gtk_combo_box_text_get_active_text(midi_dialog->jack_server);
    
    pthread_mutex_lock(application_mutex);
    
    list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
    list = ags_jack_server_find_url(list,
				    server);

    if(list != NULL){
      jack_server = list->data;
    }else{
      jack_server = NULL;
    }
    
    pthread_mutex_unlock(application_mutex);

    /* add client */
    jack_client = ags_jack_client_new(jack_server);
    ags_jack_server_add_client(jack_server,
			       jack_client);
    
    /* fill combo box and open client */
    client = gtk_entry_get_text(midi_dialog->client_name);

    gtk_combo_box_text_append_text(midi_dialog->jack_client,
				   client);
    
    ags_jack_client_open(jack_client,
			 client);
  }
  
  return(0);
}

int
ags_midi_dialog_remove_client_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    gtk_combo_box_text_remove(midi_dialog->jack_client,
			      gtk_combo_box_get_active(midi_dialog->jack_client));
  }
  
  return(0);
}

int
ags_midi_dialog_add_port_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(str == NULL ||
     g_utf8_strlen(str,
		   -1) == 0){
    return(0);
  }
  
  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    AgsWindow *window;
    AgsMachine *machine;

    GObject *sequencer;
    
    AgsJackServer *jack_server;
    AgsJackClient *jack_client;
    AgsJackPort *jack_port;
    
    AgsMutexManager *mutex_manager;

    AgsApplicationContext *application_context;

    GList *list;
    
    gchar *server;
    gchar *client;
    gchar *port;

    pthread_mutex_t *application_mutex;
    
    machine = midi_dialog->machine;

    window = gtk_widget_get_ancestor(machine,
				     AGS_TYPE_WINDOW);

    /* application context and mutex manager */
    application_context = (AgsApplicationContext *) window->application_context;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* find server */
    server = gtk_combo_box_text_get_active_text(midi_dialog->jack_server);
    
    pthread_mutex_lock(application_mutex);
    
    list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
    list = ags_jack_server_find_url(list,
				    server);

    if(list != NULL){
      jack_server = list->data;
    }else{
      jack_server = NULL;
    }
    
    pthread_mutex_unlock(application_mutex);

    /* find client */
    if(jack_server != NULL){
      client = gtk_combo_box_text_get_active_text(midi_dialog->jack_client);

      list = jack_server->client;
      list = ags_jack_client_find(list,
				  client);

      if(list != NULL){
	jack_client = list->data;
      }else{
	jack_client = NULL;
      }
    }
    
    /* add port */
    jack_port = ags_jack_port_new(jack_client);
    ags_jack_client_add_port(jack_client,
			     jack_port);

    /* fill combo box and register port */
    port = gtk_entry_get_text(midi_dialog->port_name);

    gtk_combo_box_text_append_text(midi_dialog->midi_device,
				   port);

    ags_jack_port_register(jack_port,
			   port,
			   FALSE, TRUE,
			   FALSE);
  }
  
  return(0);
}

int
ags_midi_dialog_remove_port_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    gtk_combo_box_text_remove(midi_dialog->midi_device,
			      gtk_combo_box_get_active(midi_dialog->midi_device));
  }

  return(0);
}

int
ags_midi_dialog_apply_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  ags_applicable_apply(AGS_APPLICABLE(midi_dialog));

  //TODO:JK: remove me
  //  ags_applicable_reset(AGS_APPLICABLE(midi_dialog));

  return(0);
}

int
ags_midi_dialog_ok_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(midi_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(midi_dialog));
  ags_applicable_apply(AGS_APPLICABLE(midi_dialog));
 
  midi_dialog->machine->connection = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}

int
ags_midi_dialog_cancel_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  midi_dialog->machine->connection = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}
