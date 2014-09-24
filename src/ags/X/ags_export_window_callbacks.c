/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/ags_export_window_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_export_thread.h>

#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_export_output.h>
#include <ags/audio/task/ags_cancel_audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

void ags_export_window_export_launch_callback(AgsTask *task,
					      AgsExportWindow *export_window);
void ags_export_window_cancel_launch_callback(AgsTask *task,
					      AgsExportWindow *export_window);

void
ags_export_window_file_chooser_button_callback(GtkWidget *file_chooser_button,
					       AgsExportWindow *export_window)
{
  GtkFileChooserDialog *file_chooser;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("Export to file ...\0",
								      GTK_WINDOW(export_window),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
								      NULL);
  if(gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT){
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    gtk_entry_set_text(export_window->filename,
		       filename);
  }
  
  gtk_widget_destroy(file_chooser);
}

void
ags_export_window_tact_callback(GtkWidget *spin_button,
				AgsExportWindow *export_window)
{
  gtk_label_set_text(export_window->duration,
		     ags_navigation_tact_to_time_string(gtk_spin_button_get_value(export_window->tact)));
}

void
ags_export_window_export_callback(GtkWidget *toggle_button,
				  AgsExportWindow *export_window)
{
  AgsAudioLoop *audio_loop;
  AgsWindow *window;
  AgsMachine *machine;
  GList *machines_start;

  window = AGS_MAIN(export_window->ags_main)->window;
  audio_loop = AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop);
  
  machines_start = NULL;

  if(gtk_toggle_button_get_active(toggle_button)){
    AgsInitAudio *init_audio;
    AgsAppendAudio *append_audio;
    AgsStartDevout *start_devout;
    AgsExportOutput *export_output;
    AgsExportThread *export_thread;
    GList *machines;
    GList *list;
    gchar *filename;
    gboolean live_performance;

    export_thread = audio_loop->export_thread;

    filename = gtk_entry_get_text(export_window->filename);

    if(filename == NULL ||
       strlen(filename) == 0){
      return;
    }

    live_performance = gtk_toggle_button_get_active(export_window->live_export);

    machines_start = 
      machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

    list = NULL;

    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	printf("found machine to play!\n\0");

	/* create init task */
	init_audio = ags_init_audio_new(machine->audio,
					FALSE, TRUE, TRUE);
	list = g_list_prepend(list, init_audio);
    
	/* create append task */
	append_audio = ags_append_audio_new(audio_loop,
					    (GObject *) machine->audio);
      
	list = g_list_prepend(list, append_audio);
      }

      machines = machines->next;
    }

    /* create start task */
    if(list != NULL){
      start_devout = ags_start_devout_new(window->devout);
      export_output = ags_export_output_new(export_thread,
					    window->devout,
					    filename,
					    live_performance);
      g_signal_connect(export_output, "launch\0",
		       G_CALLBACK(ags_export_window_export_launch_callback), export_window);
      list = g_list_prepend(list, start_devout);
      list = g_list_reverse(list);

      /* append AgsStartDevout */
      ags_task_thread_append_tasks(AGS_TASK_THREAD(audio_loop->task_thread),
				   list);
    }
  }else{
    AgsCancelAudio *cancel_audio;
    GList *machines;
    GList *list;

    machines_start = 
      machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

    list = NULL;

    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	printf("found machine to stop!\n\0");
    
	/* create append task */
	cancel_audio = ags_cancel_audio_new(machine->audio,
					    FALSE, FALSE, TRUE);
      
	list = g_list_prepend(list, cancel_audio);
      }

      machines = machines->next;
    }

    /* create start task */
    if(list != NULL){
      g_signal_connect(list->data, "launch\0",
		       G_CALLBACK(ags_export_window_cancel_launch_callback), export_window);      
      list = g_list_reverse(list);

      /* append AgsStartDevout */
      ags_task_thread_append_tasks(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
				   list);
    }  
  }

  g_list_free(machines_start);
}

void
ags_export_window_export_launch_callback(AgsTask *task,
					 AgsExportWindow *export_window)
{
  AgsWindow *window;

  window = AGS_MAIN(export_window->ags_main)->window;
  ags_navigation_set_seeking_sensitive(window->navigation,
				       FALSE);
}

void
ags_export_window_cancel_launch_callback(AgsTask *task,
					 AgsExportWindow *export_window)
{
  AgsWindow *window;

  window = AGS_MAIN(export_window->ags_main)->window;
  ags_navigation_set_seeking_sensitive(window->navigation,
				       TRUE);
}
