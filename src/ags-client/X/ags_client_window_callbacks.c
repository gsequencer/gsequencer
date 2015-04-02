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

#include <ags-client/X/ags_client_window_callbacks.h>

gboolean ags_client_window_delete_event_callback(GtkWidget *widget,
						 gpointer data)
{

  AgsClientWindow *client_window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;
  gint response;

  client_window = AGS_CLIENT_WINDOW(widget);

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

  return(TRUE);

}
