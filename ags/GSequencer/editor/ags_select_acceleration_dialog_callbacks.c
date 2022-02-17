/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/GSequencer/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_automation_window.h>
#include <ags/GSequencer/ags_automation_editor.h>
#include <ags/GSequencer/ags_machine.h>

#include <ags/i18n.h>

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
  AgsMachine *machine;

  GtkHBox *hbox;
  GtkComboBoxText *combo_box;
  GtkCellRenderer *scope_cell_renderer_text;
  GtkCellRenderer *port_cell_renderer_text;
  GtkButton *remove;

  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;

  AgsApplicationContext *application_context;

  GtkListStore *list_store;

  GtkTreeIter iter;
  
  GList *start_port, *port;

  gchar **collected_audio_specifier, **collected_input_specifier, **collected_output_specifier;

  gboolean use_composite_editor;
  guint length;

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;

  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;

    machine = automation_editor->selected_machine;
  }
  
  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;

  /* select automation */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) select_acceleration_dialog->port,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* automation combo box */
  combo_box = (GtkComboBox *) gtk_combo_box_new();

  list_store = gtk_list_store_new(2,
				  G_TYPE_STRING,
				  G_TYPE_STRING);
  
  gtk_combo_box_set_model(combo_box,
			  GTK_TREE_MODEL(list_store));

  scope_cell_renderer_text = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box),
			     scope_cell_renderer_text,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), scope_cell_renderer_text,
				 "text", 0,
				 NULL);

  port_cell_renderer_text = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box),
			     port_cell_renderer_text,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), port_cell_renderer_text,
				 "text", 1,
				 NULL);
  
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  /*  */  
  collected_audio_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_audio_specifier[0] = NULL;
  length = 1;
  
  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(audio);

  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean is_enabled;
    gboolean contains_control_name;

    plugin_port = NULL;
    
    specifier = NULL;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(collected_audio_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_audio_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){
      gtk_list_store_append(list_store, &iter);
      gtk_list_store_set(list_store, &iter,
			 0, g_strdup("audio"),
			 1, g_strdup(specifier),
			 -1);      

      /* add to collected specifier */
      collected_audio_specifier = (gchar **) realloc(collected_audio_specifier,
						     (length + 1) * sizeof(gchar *));
      collected_audio_specifier[length - 1] = g_strdup(specifier);
      collected_audio_specifier[length] = NULL;

      length++;
    }

    if(plugin_port != NULL){
      g_object_unref(plugin_port);
    }

    g_free(specifier);
    
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

  collected_output_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_output_specifier[0] = NULL;
  length = 1;

  while(channel != NULL){
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      plugin_port = NULL;
    
      specifier = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_output_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_output_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, g_strdup("output"),
			   1, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_output_specifier = (gchar **) realloc(collected_output_specifier,
							(length + 1) * sizeof(gchar *));
	collected_output_specifier[length - 1] = g_strdup(specifier);
	collected_output_specifier[length] = NULL;

	length++;
      }

      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }

      g_free(specifier);
    
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

  collected_input_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_input_specifier[0] = NULL;
  length = 1;

  while(channel != NULL){
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      plugin_port = NULL;
    
      specifier = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_input_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_input_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, g_strdup("input"),
			   1, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_input_specifier = (gchar **) realloc(collected_input_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_input_specifier[length - 1] = g_strdup(specifier);
	collected_input_specifier[length] = NULL;

	length++;
      }

      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }

      g_free(specifier);
    
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
  
  g_strfreev(collected_audio_specifier);
  g_strfreev(collected_output_specifier);
  g_strfreev(collected_input_specifier);
  
  /* remove button */
  remove = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));
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
