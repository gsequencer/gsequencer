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

#include <ags/X/ags_menu_bar_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_applicable.h>

#include <ags/file/ags_file.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>

#include <ags/audio/task/ags_save_file.h>
#include <ags/audio/task/ags_add_audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_export_window.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include <X11/Xlib.h>

void ags_menu_bar_open_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar);
void ags_menu_bar_open_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar);

gboolean
ags_menu_bar_destroy_callback(GtkObject *object, AgsMenuBar *menu_bar)
{
  ags_menu_bar_destroy(object);

  return(TRUE);
}

void
ags_menu_bar_show_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  ags_menu_bar_show(widget);
}

void
ags_menu_bar_open_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_file_selection_new(g_strdup("open file\0"));
  gtk_file_selection_set_select_multiple(file_selection, FALSE);

  gtk_widget_show_all((GtkWidget *) file_selection);

  g_signal_connect((GObject *) file_selection->ok_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_open_ok_callback), menu_bar);
  g_signal_connect((GObject *) file_selection->cancel_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_open_cancel_callback), menu_bar);
}

void
ags_menu_bar_open_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkFileSelection *file_selection;
  char *filename;
  gchar **argv;
  GError *error;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);
  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);

  filename = g_strdup(gtk_file_selection_get_filename(file_selection));

  error = NULL;

  g_spawn_command_line_async(g_strdup_printf("./gsequencer --filename %s\0",
					     filename),
			     &error);

  gtk_widget_destroy((GtkWidget *) file_selection);

  g_free(filename);
}

void
ags_menu_bar_open_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_menu_bar_save_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsFile *file;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  //TODO:JK: revise me
  file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				  "main\0", window->ags_main,
				  "filename\0", g_strdup(window->name),
				  NULL);
  ags_file_rw_open(file,
		   TRUE);
  ags_file_write(file);
  ags_file_close(file);
  g_object_unref(G_OBJECT(file));
}

void
ags_menu_bar_save_as_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;
  gint response;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("save file as\0",
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_chooser), TRUE);
  gtk_widget_show_all((GtkWidget *) file_chooser);

  response = gtk_dialog_run(GTK_DIALOG(file_chooser));

  if(response == GTK_RESPONSE_ACCEPT){
    AgsFile *file;
    AgsSaveFile *save_file;
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "main\0", window->ags_main,
				    "filename\0", filename,
				    NULL);

    window->name = filename;
    gtk_window_set_title((GtkWindow *) window,
			 g_strconcat("GSequencer - \0",
				     window->name,
				     NULL));
    
    save_file = ags_save_file_new(file);
    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
				AGS_TASK(save_file));
  }

  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_menu_bar_export_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  gtk_widget_show_all((GtkWidget *) window->export_window);
}

void
ags_menu_bar_quit_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;
  gint response;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

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

    //TODO:JK: revise me
    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "main\0", window->ags_main,
				    "filename\0", g_strdup(window->name),
				    NULL);

    ags_file_write(file);
    g_object_unref(G_OBJECT(file));
  }

  if(response != GTK_RESPONSE_CANCEL){
    ags_main_quit(AGS_MAIN(window->ags_main));
  }else{
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
}


void
ags_menu_bar_add_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
}


void
ags_menu_bar_add_panel_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsPanel *panel;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  panel = ags_panel_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(panel)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(panel),
		     FALSE, FALSE, 0);

  AGS_MACHINE(panel)->audio->audio_channels = 2;
  
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_INPUT, 1);
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_OUTPUT, 1);

  ags_connectable_connect(AGS_CONNECTABLE(panel));

  gtk_widget_show_all(GTK_WIDGET(panel));
}

void
ags_menu_bar_add_mixer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsMixer *mixer;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  mixer = ags_mixer_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(mixer)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(mixer),
		     FALSE, FALSE, 0);

  mixer->machine.audio->audio_channels = 2;
  ags_audio_set_pads(mixer->machine.audio,
		     AGS_TYPE_INPUT, 8);
  ags_audio_set_pads(mixer->machine.audio,
		     AGS_TYPE_OUTPUT, 1);

  ags_connectable_connect(AGS_CONNECTABLE(mixer));

  gtk_widget_show_all(GTK_WIDGET(mixer));
}

void
ags_menu_bar_add_drum_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsDrum *drum;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  drum = ags_drum_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(drum)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(drum),
		     FALSE, FALSE, 0);

  /* */
  drum->machine.audio->audio_channels = 2;

  /* AgsDrumInputPad */
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_INPUT, 8);
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(drum));

  /* */
  gtk_widget_show_all(GTK_WIDGET(drum));
}

void
ags_menu_bar_add_matrix_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsMatrix *matrix;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  matrix = ags_matrix_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(matrix)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(matrix),
		     FALSE, FALSE, 0);
  
  /* */
  matrix->machine.audio->audio_channels = 1;

  /* AgsMatrixInputPad */
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_INPUT, 78);
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(matrix));

  /* */
  gtk_widget_show_all(GTK_WIDGET(matrix));
}

void
ags_menu_bar_add_synth_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsSynth *synth;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  synth = ags_synth_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(synth)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) synth,
		     FALSE, FALSE, 0);

  synth->machine.audio->audio_channels = 1;
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_INPUT, 2);
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_OUTPUT, 78);

  ags_connectable_connect(AGS_CONNECTABLE(synth));

  gtk_widget_show_all((GtkWidget *) synth);
}

void
ags_menu_bar_add_ffplayer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;
  AgsAddAudio *add_audio;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  ffplayer = ags_ffplayer_new(G_OBJECT(window->devout));

  add_audio = ags_add_audio_new(window->devout,
				AGS_MACHINE(ffplayer)->audio);
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) ffplayer,
		     FALSE, FALSE, 0);

    //  ffplayer->machine.audio->frequence = ;
  ffplayer->machine.audio->audio_channels = 2;
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio, AGS_TYPE_INPUT, 78);
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio, AGS_TYPE_OUTPUT, 1);

  ags_connectable_connect(AGS_CONNECTABLE(ffplayer));
  gtk_widget_show_all((GtkWidget *) ffplayer);
}


void
ags_menu_bar_remove_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
}

void
ags_menu_bar_preferences_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  if(window->preferences != NULL){
    return;
  }

  window->preferences = ags_preferences_new();
  window->preferences->window = GTK_WINDOW(window);

  ags_applicable_reset(AGS_APPLICABLE(window->preferences));

  ags_connectable_connect(AGS_CONNECTABLE(window->preferences));
  gtk_widget_show_all(GTK_WIDGET(window->preferences));
}

void
ags_menu_bar_about_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  static FILE *file = NULL;
  struct stat sb;
  static gchar *license;
  static GdkPixbuf *logo;
  GError *error;

  gchar *authors[] = { "Joël Krähemann\0", NULL }; 

  if(g_file_test("/usr/share/common-licenses/GPL-3\0",
		 G_FILE_TEST_EXISTS)){
    if(file == NULL){
      file = fopen("/usr/share/common-licenses/GPL-3\0", "r\0");
      stat("/usr/share/common-licenses/GPL-3\0", &sb);
      license = (gchar *) malloc((sb.st_size + 1) * sizeof(gchar));
      fread(license, sizeof(char), sb.st_size, file);
      license[sb.st_size] = '\0';
      fclose(file);

      error = NULL;

      logo = gdk_pixbuf_new_from_file(g_strdup_printf("%s%s\0", DESTDIR, "/gsequencer/images/ags.png\0"), &error);
    }
  }
  
  gtk_show_about_dialog((GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, GTK_TYPE_WINDOW),
			"program-name\0", "gsequencer\0",
			"authors\0", authors,
			"license\0", license,
			"version\0", AGS_VERSION,
			"website\0", "http://gsequencer.org\0",
			"title\0", "Advanced Gtk+ Sequencer\0",
			"logo\0", logo,
			NULL);
}
