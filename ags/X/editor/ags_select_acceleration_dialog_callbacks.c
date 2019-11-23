/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

void
ags_select_acceleration_dialog_response_callback(GtkWidget *dialog, gint response,
						 AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  gboolean hide_dialog;

  hide_dialog = TRUE;
  
  switch(response){
  case GTK_RESPONSE_APPLY:
    {
      hide_dialog = FALSE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(select_acceleration_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide((GtkWidget *) select_acceleration_dialog);
      }
    }
  }
}

void
ags_select_acceleration_dialog_add_callback(GtkWidget *button,
					    AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;

  GtkHBox *hbox;
  GtkComboBoxText *combo_box;
  GtkButton *remove;

  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  GList *start_port, *port;

  gchar **collected_specifier;

  guint length;

  window = AGS_WINDOW(select_acceleration_dialog->main_window);
  automation_editor = window->automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  audio = machine->audio;

  /* select automation */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) select_acceleration_dialog->port,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* automation combo box */
  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  /*  */  
  collected_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(audio);

  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean is_enabled;
    gboolean contains_control_name;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){
      gtk_combo_box_text_append_text(combo_box,
				     g_strdup(specifier));

      /* add to collected specifier */
      collected_specifier = (gchar **) realloc(collected_specifier,
				     (length + 1) * sizeof(gchar *));
      collected_specifier[length - 1] = g_strdup(specifier);
      collected_specifier[length] = NULL;

      length++;
    }
    
    /* iterate */
    port = port->next;
  }

  g_list_free_full(start_port,
		   g_object_unref);
    
  /* output */
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;
  g_object_ref(channel);

  next_channel = NULL;

  while(channel != NULL){
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(combo_box,
				       g_strdup(specifier));

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  /* input */
  g_object_get(audio,
	       "input", &start_channel,
	       NULL);

  channel = start_channel;
  g_object_ref(channel);

  next_channel = NULL;

  while(channel != NULL){
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(combo_box,
				       g_strdup(specifier));

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_strfreev(collected_specifier);
  
  /* remove button */
  remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) remove,
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all((GtkWidget *) hbox);
  
  g_signal_connect(remove, "clicked",
		   G_CALLBACK(ags_select_acceleration_dialog_remove_callback), select_acceleration_dialog);
}

void
ags_select_acceleration_dialog_remove_callback(GtkWidget *button,
					       AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  gtk_widget_destroy(gtk_widget_get_parent(button));
}

void
ags_select_acceleration_dialog_machine_changed_callback(AgsAutomationEditor *automation_editor,
							AgsMachine *machine,
							AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  ags_applicable_reset(AGS_APPLICABLE(select_acceleration_dialog));
}
