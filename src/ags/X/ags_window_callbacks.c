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

#include <ags/X/ags_window_callbacks.h>

#include <ags/file/ags_file.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/task/ags_save_file.h>

gboolean
ags_window_delete_event_callback(GtkWidget *widget, gpointer data)
{
  AgsWindow *window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;

  AgsMutexManager *mutex_manager;
  
  gint response;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(widget);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* ask the user if he wants save to a file */
  dialog = (GtkDialog *) gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						"Do you want to save '%s'?\0", window->name);
  cancel_button = gtk_dialog_add_button(dialog,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL);
  gtk_widget_grab_focus(cancel_button);

  response = gtk_dialog_run(dialog);

  if(response == GTK_RESPONSE_YES){
    AgsFile *file;
    AgsSaveFile *save_file;

    AgsThread *audio_loop;
    AgsThread *task_thread;
    
    AgsApplicationContext *application_context;
    
    char *filename;

    application_context = window->application_context;

    /* get audio loop */
    pthread_mutex_lock(application_mutex);

    audio_loop = application_context->main_loop;

    pthread_mutex_unlock(application_mutex);

    /* get task thread */
    task_thread = (AgsTaskThread *) ags_thread_find_type(audio_loop,
							 AGS_TYPE_TASK_THREAD);


    filename = window->name;

    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "main\0", window->application_context,
				    "filename\0", g_strdup(filename),
				    NULL);

    save_file = ags_save_file_new(file);
    ags_task_thread_append_task(task_thread,
				AGS_TASK(save_file));

    g_object_unref(G_OBJECT(file));
  }

  if(response != GTK_RESPONSE_CANCEL){
    application_context_quit(AGS_APPLLICATION_CONTEXT(window->application_context));
  }else{
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }

  return(TRUE);
}

