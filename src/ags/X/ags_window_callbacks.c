/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_window_callbacks.h>

#include <ags/object/ags_application_context.h>

#include <ags/file/ags_file.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/file/task/ags_save_file.h>

gboolean
ags_window_delete_event_callback(GtkWidget *widget, gpointer data)
{
  AgsWindow *window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;

  AgsApplicationContext *application_context;

  gint response;

  window = AGS_WINDOW(widget);

  application_context = window->application_context;

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
    AgsThread *main_loop;
    AgsTaskThread *task_thread;
    AgsSaveFile *save_file;
    
    AgsFile *file;
    
    char *filename;
    
    main_loop = application_context->main_loop;
    
    task_thread = ags_thread_find_type(main_loop,
				       AGS_TYPE_TASK_THREAD);

    filename = window->name;

    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "application-context\0", application_context,
				    "filename\0", g_strdup(filename),
				    NULL);

    save_file = ags_save_file_new(file);
    ags_task_thread_append_task(task_thread,
				AGS_TASK(save_file));

    g_object_unref(G_OBJECT(file));
  }

  if(response != GTK_RESPONSE_CANCEL){
    ags_main_quit(application_context);
  }else{
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }

  return(TRUE);
}

