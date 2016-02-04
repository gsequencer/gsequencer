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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

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
  //TODO:JK: implement me
  
  return(0);
}

int
ags_midi_dialog_remove_server_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  //TODO:JK: implement me
  
  return(0);
}

int
ags_midi_dialog_add_client_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  //TODO:JK: implement me
  
  return(0);
}

int
ags_midi_dialog_remove_client_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  //TODO:JK: implement me
  
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
    gchar *port;

    port = gtk_entry_get_text(midi_dialog->port_name);

    gtk_combo_box_text_append_text(midi_dialog->midi_device,
				   port);
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
