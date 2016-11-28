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
#include <ags/object/ags_distributed_manager.h>

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

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(midi_dialog->backend));

  if(!g_ascii_strncasecmp("alsa\0",
			  str,
			  4)){
    /* insensitive for alsa */
    gtk_widget_set_sensitive((GtkWidget *) midi_dialog->add_sequencer,
			     FALSE);
    gtk_widget_set_sensitive((GtkWidget *) midi_dialog->remove_sequencer,
			     FALSE);
  }else if(!g_ascii_strncasecmp("jack\0",
				str,
				4)){
    /* sensitive for jack */
    gtk_widget_set_sensitive((GtkWidget *) midi_dialog->add_sequencer,
			     TRUE);
    gtk_widget_set_sensitive((GtkWidget *) midi_dialog->remove_sequencer,
			     TRUE);
  }
  
  ags_midi_dialog_load_sequencers(midi_dialog);

  return(0);
}

int
ags_midi_dialog_add_sequencer_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  AgsWindow *window;
  AgsMachine *machine;
    
  AgsMutexManager *mutex_manager;
  AgsApplicationContext *application_context;

  GObject *sequencer;

  gchar *str;

  pthread_mutex_t *application_mutex;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(midi_dialog->backend));

  if(str == NULL ||
     g_utf8_strlen(str,
		   -1) == 0){
    return(0);
  }
    
  machine = midi_dialog->machine;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  /* application context and mutex manager */
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    AgsJackServer *jack_server;
    
    GList *distributed_manager;

    /* find server */
    pthread_mutex_lock(application_mutex);
    
    distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
    
    if(distributed_manager != NULL){
      jack_server = AGS_JACK_SERVER(distributed_manager->data);
    }else{
      g_warning("distributed manager not found\0");
      
      pthread_mutex_unlock(application_mutex);
      
      return(0);
    }

    sequencer = ags_distributed_manager_register_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
							   FALSE);

    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_append(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						   sequencer));
    g_object_ref(sequencer);

    pthread_mutex_unlock(application_mutex);

    /* load midi devices */
    ags_midi_dialog_load_sequencers(midi_dialog);
  }
  
  return(0);
}

int
ags_midi_dialog_remove_sequencer_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsMutexManager *mutex_manager;
  AgsApplicationContext *application_context;
    
  GObject *sequencer;    

  gchar *str;

  pthread_mutex_t *application_mutex;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(midi_dialog->backend));

  if(str == NULL ||
     g_utf8_strlen(str,
		   -1) == 0){
    return(0);
  }

  machine = midi_dialog->machine;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  /* application context and mutex manager */
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  if(!g_ascii_strncasecmp("jack\0",
			  str,
			  4)){
    AgsJackServer *jack_server;
    
    GList *distributed_manager;
    GList *list;

    guint i;
        
    pthread_mutex_lock(application_mutex);
    
    /* find server */
    distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

    if(distributed_manager != NULL){
      jack_server = distributed_manager->data;
    }else{
      jack_server = NULL;
    }

    /* find sequencer */
    sequencer = NULL;
    
    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

    for(i = 0; list != NULL && i < gtk_combo_box_get_active(midi_dialog->midi_device) - 1;){
      if(AGS_IS_JACK_MIDIIN(list->data)){
	i++;
      }
      
      list = list->next;
    }

    if(list != NULL){
      sequencer = G_OBJECT(list->data);
    }

    /* unregister */
    ags_distributed_manager_unregister_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
						 sequencer);
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						   sequencer));
    g_object_unref(sequencer);
    
    pthread_mutex_unlock(application_mutex);
    
    /* load midi devices */
    ags_midi_dialog_load_sequencers(midi_dialog);
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
 
  midi_dialog->machine->midi_dialog = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}

int
ags_midi_dialog_cancel_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  midi_dialog->machine->midi_dialog = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}
