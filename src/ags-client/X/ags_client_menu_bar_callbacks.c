/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags-client/X/ags_client_menu_bar_callbacks.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <ags-client/X/ags_client_window.h>

void ags_client_menu_bar_open_ok_callback(GtkWidget *widget, AgsClientMenuBar *client_menu_bar);
void ags_client_menu_bar_open_cancel_callback(GtkWidget *widget, AgsClientMenuBar *client_menu_bar);

void
ags_client_menu_bar_open_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_file_selection_new(g_strdup("open file\0"));
  gtk_file_selection_set_select_multiple(file_selection, FALSE);

  gtk_widget_show_all((GtkWidget *) file_selection);

  g_signal_connect((GObject *) file_selection->ok_button, "clicked\0",
		   G_CALLBACK(ags_client_menu_bar_open_ok_callback), client_menu_bar);
  g_signal_connect((GObject *) file_selection->cancel_button, "clicked\0",
		   G_CALLBACK(ags_client_menu_bar_open_cancel_callback), client_menu_bar);
}

void
ags_client_menu_bar_open_ok_callback(GtkWidget *widget, AgsClientMenuBar *client_menu_bar)
{
  GtkFileSelection *file_selection;
  AgsFile *file;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);

//TODO:JK: implement me

  g_object_unref(G_OBJECT(file));
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_client_menu_bar_open_cancel_callback(GtkWidget *widget, AgsClientMenuBar *client_menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_client_menu_bar_save_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
}

void
ags_client_menu_bar_save_as_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
}

void
ags_client_menu_bar_quit_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
  AgsClientWindow *client_window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;
  gint response;

  client_window = (AgsClientWindow *) gtk_widget_get_toplevel((GtkWidget *) client_menu_bar);

  /* ask the user if he wants save to a file */
  dialog = (GtkDialog *) gtk_message_dialog_new(GTK_WINDOW(client_window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						"Do you want to save '%s'?\0", client_window->name);
  cancel_button = gtk_dialog_add_button(dialog,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL);
  gtk_widget_grab_focus(cancel_button);

  response = gtk_dialog_run(dialog);

  if(response == GTK_RESPONSE_YES){
    //TODO:JK: implement me
  }

  if(response != GTK_RESPONSE_CANCEL){
    gtk_main_quit();
  }else{
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
}

void
ags_client_menu_bar_connect_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
}

void
ags_client_menu_bar_execute_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
}

void
ags_client_menu_bar_about_callback(GtkWidget *menu_item, AgsClientMenuBar *client_menu_bar)
{
  static FILE *file = NULL;
  struct stat sb;
  static gchar *license;

  gchar *authors[] = { "joel kraehemann\0", NULL }; 

  if(file == NULL){
    file = fopen("./COPYING\0", "r\0");
    stat("./COPYING\0", &sb);
    license = (gchar *) malloc((sb.st_size + 1) * sizeof(gchar));
    fread(license, sizeof(char), sb.st_size, file);
    license[sb.st_size] = '\0';
    fclose(file);
  }

  gtk_show_about_dialog((GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) client_menu_bar, GTK_TYPE_WINDOW),
			"program-name\0", "agsclient\0",
			"authors\0", authors,
			"license\0", license,
			"title\0", "ags\0",
			NULL);
}
