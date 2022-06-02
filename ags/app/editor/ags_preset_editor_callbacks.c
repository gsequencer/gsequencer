/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/editor/ags_preset_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_input_dialog.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_preset_dialog.h>

#include <ags/i18n.h>

void ags_preset_editor_open_response_callback(GtkDialog *dialog, gint response, AgsPresetEditor *preset_editor);

void
ags_preset_editor_load_callback(GtkButton *button, AgsPresetEditor *preset_editor)
{
  AgsMachine *machine;
  AgsPresetDialog *preset_dialog;
  
  GtkListStore *list_store;
  GtkTreeIter iter;

  AgsChannel *start_channel;
  AgsChannel *channel;

  GList *start_port, *port;
  GList *start_context_specifier;

  gchar **collected_specifier;

  guint length;
  
  preset_dialog = (AgsPresetDialog *) gtk_widget_get_ancestor((GtkWidget *) preset_editor,
							      AGS_TYPE_PRESET_DIALOG);
  machine = preset_dialog->machine;

  if(machine == NULL){
    gtk_tree_view_set_model(preset_editor->preset_tree_view,
			    NULL);
    
    return;
  }

  collected_specifier = (gchar **) g_malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;

  list_store = gtk_list_store_new(6,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_GTYPE);

  gtk_tree_view_set_model(preset_editor->preset_tree_view,
			  GTK_TREE_MODEL(list_store));

  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(machine->audio);

  start_context_specifier = NULL;
  
  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean is_enabled;
    gboolean contains_control_name;

    specifier = NULL;
    plugin_port = NULL;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

    if(specifier == NULL){
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
      
      /* iterate */
      port = port->next;

      continue;
    }

#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif
    
    if(plugin_port != NULL &&
       g_list_find_custom(start_context_specifier, specifier, g_strcmp0) == NULL){
      GValue port_value = G_VALUE_INIT;

      start_context_specifier = g_list_prepend(start_context_specifier,
					       specifier);
      
      /* create list store entry */
      if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
	g_value_init(&port_value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port->data,
			    &port_value);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, g_strdup("audio"),
			   1, g_strdup("0"),
			   2, g_strdup(specifier),
			   3, g_strdup_printf("%f - %f",
					      g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value)),
			   4, g_strdup_printf("%f",
					      g_value_get_float(&port_value)),
			   5, G_TYPE_FLOAT,
			   -1);      
      }else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
	g_value_init(&port_value,
		     G_TYPE_DOUBLE);

	ags_port_safe_read(port->data,
			    &port_value);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, g_strdup("audio"),
			   1, g_strdup("0"),
			   2, g_strdup(specifier),
			   3, g_strdup_printf("%f - %f",
					      g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value)),
			   4, g_strdup_printf("%f",
					      g_value_get_double(&port_value)),
			   5, G_TYPE_DOUBLE,
			   -1);      
      }
      
      /* add to collected specifier */
      if(!contains_control_name){
	collected_specifier = (gchar **) g_realloc(collected_specifier,
						   (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }
    }else{
      g_free(specifier);
    }
    
    if(plugin_port != NULL){
      g_object_unref(plugin_port);
    }
    
    /* iterate */
    port = port->next;
  }
  
  g_strfreev(collected_specifier);

  g_list_free_full(start_context_specifier,
		   g_free);
  g_list_free_full(start_port,
		   g_object_unref);

  /* output */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  collected_specifier = (gchar **) g_malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  while(channel != NULL){
    AgsChannel *next;

    guint line;
    
    line = ags_channel_get_line(channel);

    start_context_specifier = NULL;
    
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      GValue port_value = G_VALUE_INIT;
      
      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	goto OUTPUT_ITERATE;
      }
      
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 g_list_find_custom(start_context_specifier, specifier, g_strcmp0) == NULL){
	start_context_specifier = g_list_prepend(start_context_specifier,
						 specifier);
	
	/* create list store entry */
	if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
	  g_value_init(&port_value,
		       G_TYPE_FLOAT);
	  
	  ags_port_safe_read(port->data,
			     &port_value);

	  gtk_list_store_append(list_store, &iter);
	  gtk_list_store_set(list_store, &iter,
			     0, g_strdup("output"),
			     1, g_strdup_printf("%d", line),
			     2, g_strdup(specifier),
			     3, g_strdup_printf("%f - %f",
						g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value)),
			     4, g_strdup_printf("%f",
						g_value_get_float(&port_value)),
			     5, G_TYPE_FLOAT,
			     -1);      
	}else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
	  g_value_init(&port_value,
		       G_TYPE_DOUBLE);
	  
	  ags_port_safe_read(port->data,
			     &port_value);

	  gtk_list_store_append(list_store, &iter);
	  gtk_list_store_set(list_store, &iter,
			     0, g_strdup("output"),
			     1, g_strdup_printf("%d", line),
			     2, g_strdup(specifier),
			     3, g_strdup_printf("%f - %f",
						g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value)),
			     4, g_strdup_printf("%f",
						g_value_get_double(&port_value)),
			     5, G_TYPE_DOUBLE,
			     -1);      
	}

	/* add to collected specifier */
	if(!contains_control_name){
	  collected_specifier = (gchar **) g_realloc(collected_specifier,
						     (length + 1) * sizeof(gchar *));
	  collected_specifier[length - 1] = g_strdup(specifier);
	  collected_specifier[length] = NULL;
	  
	  length++;
	}
      }else{
	g_free(specifier);
      }
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }

    OUTPUT_ITERATE:
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_context_specifier,
		     g_free);

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  /* input */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "input", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    guint line;
    
    line = ags_channel_get_line(channel);

    start_context_specifier = NULL;
    
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsChannel *next;
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      GValue port_value = G_VALUE_INIT;
      
      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	port = port->next;

	continue;
      }
      
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 g_list_find_custom(start_context_specifier, specifier, g_strcmp0) == NULL){
	start_context_specifier = g_list_prepend(start_context_specifier,
						 specifier);
	
	/* create list store entry */
	if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
	  g_value_init(&port_value,
		       G_TYPE_FLOAT);

	  ags_port_safe_read(port->data,
			     &port_value);

	  gtk_list_store_append(list_store, &iter);
	  gtk_list_store_set(list_store, &iter,
			     0, g_strdup("input"),
			     1, g_strdup_printf("%d", line),
			     2, g_strdup(specifier),
			     3, g_strdup_printf("%f - %f",
						g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value)),
			     4, g_strdup_printf("%f",
						g_value_get_float(&port_value)),
			     5, G_TYPE_FLOAT,
			     -1);      
	}else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
	  g_value_init(&port_value,
		       G_TYPE_DOUBLE);

	  ags_port_safe_read(port->data,
			     &port_value);

	  gtk_list_store_append(list_store, &iter);
	  gtk_list_store_set(list_store, &iter,
			     0, g_strdup("input"),
			     1, g_strdup_printf("%d", line),
			     2, g_strdup(specifier),
			     3, g_strdup_printf("%f - %f",
						g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value)),
			     4, g_strdup_printf("%f",
						g_value_get_double(&port_value)),
			     5, G_TYPE_DOUBLE,
			     -1);      
	}

	/* add to collected specifier */
	if(!contains_control_name){
	  collected_specifier = (gchar **) g_realloc(collected_specifier,
						     (length + 1) * sizeof(gchar *));
	  collected_specifier[length - 1] = g_strdup(specifier);
	  collected_specifier[length] = NULL;

	  length++;
	}
      }else{
	g_free(specifier);
      }
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_context_specifier,
		     g_free);

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  g_strfreev(collected_specifier);
}

void
ags_preset_editor_save_response_callback(GtkDialog *dialog, gint response, AgsPresetEditor *preset_editor)
{
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      GFile *file;
      
      gchar *filename;

      file = gtk_file_chooser_get_file(dialog);

      filename = g_file_get_path(file);
  
      gtk_editable_set_text(GTK_EDITABLE(preset_editor->filename),
			    filename);

      if(file != NULL){
	g_object_unref(file);
      }
      
      g_free(filename);
    }
  }

  gtk_window_destroy(dialog);
}

void
ags_preset_editor_save_preset_callback(GtkButton *button, AgsPresetEditor *preset_editor)
{
  ags_preset_editor_save_preset(preset_editor);
}

void
ags_preset_editor_open_response_callback(GtkDialog *dialog, gint response, AgsPresetEditor *preset_editor)
{
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      GFile *file;
      
      gchar *filename;

      file = gtk_file_chooser_get_file(dialog);

      filename = g_file_get_path(file);
  
      gtk_editable_set_text(GTK_EDITABLE(preset_editor->filename),
			    filename);

      ags_preset_editor_open_preset(preset_editor,
				    filename);

      if(file != NULL){
	g_object_unref(file);
      }
      
      g_free(filename);
    }
  }

  gtk_window_destroy(dialog);
}

void
ags_preset_editor_open_preset_callback(GtkButton *button, AgsPresetEditor *preset_editor)
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  file_chooser = gtk_file_chooser_dialog_new(i18n("open preset files"),
					     window,
					     GTK_FILE_CHOOSER_ACTION_OPEN,
					     i18n("_Cancel"), GTK_RESPONSE_CANCEL,
					     i18n("_Open"), GTK_RESPONSE_ACCEPT,
					     NULL);
  
  g_signal_connect(file_chooser, "response",
		   G_CALLBACK(ags_preset_editor_open_response_callback), preset_editor);

  gtk_widget_show(file_chooser);
}

void
ags_preset_editor_apply_preset_callback(GtkButton *button, AgsPresetEditor *preset_editor)
{
  ags_preset_editor_apply_preset(preset_editor);
}

