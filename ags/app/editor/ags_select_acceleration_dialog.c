/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/editor/ags_select_acceleration_dialog.h>
#include <ags/app/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <ags/i18n.h>

void ags_select_acceleration_dialog_class_init(AgsSelectAccelerationDialogClass *select_acceleration_dialog);
void ags_select_acceleration_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_acceleration_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_acceleration_dialog_init(AgsSelectAccelerationDialog *select_acceleration_dialog);
void ags_select_acceleration_dialog_finalize(GObject *gobject);

void ags_select_acceleration_dialog_connect(AgsConnectable *connectable);
void ags_select_acceleration_dialog_disconnect(AgsConnectable *connectable);

void ags_select_acceleration_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_acceleration_dialog_apply(AgsApplicable *applicable);
void ags_select_acceleration_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_select_acceleration_dialog
 * @short_description: select tool
 * @title: AgsSelectAccelerationDialog
 * @section_id:
 * @include: ags/app/editor/ags_select_acceleration_dialog.h
 *
 * The #AgsSelectAccelerationDialog lets you select accelerations.
 */

static gpointer ags_select_acceleration_dialog_parent_class = NULL;

GType
ags_select_acceleration_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_select_acceleration_dialog = 0;

    static const GTypeInfo ags_select_acceleration_dialog_info = {
      sizeof (AgsSelectAccelerationDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_acceleration_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectAccelerationDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_acceleration_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_acceleration_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_acceleration_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_acceleration_dialog = g_type_register_static(GTK_TYPE_DIALOG,
								 "AgsSelectAccelerationDialog", &ags_select_acceleration_dialog_info,
								 0);
    
    g_type_add_interface_static(ags_type_select_acceleration_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_acceleration_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_select_acceleration_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_select_acceleration_dialog_class_init(AgsSelectAccelerationDialogClass *select_acceleration_dialog)
{
  GObjectClass *gobject;

  ags_select_acceleration_dialog_parent_class = g_type_class_peek_parent(select_acceleration_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) select_acceleration_dialog;

  gobject->finalize = ags_select_acceleration_dialog_finalize;
}

void
ags_select_acceleration_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_select_acceleration_dialog_connect;
  connectable->disconnect = ags_select_acceleration_dialog_disconnect;
}

void
ags_select_acceleration_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_acceleration_dialog_set_update;
  applicable->apply = ags_select_acceleration_dialog_apply;
  applicable->reset = ags_select_acceleration_dialog_reset;
}

void
ags_select_acceleration_dialog_init(AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  select_acceleration_dialog->connectable_flags = 0;

  g_object_set(select_acceleration_dialog,
	       "title", i18n("select accelerations"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) select_acceleration_dialog,
			       TRUE);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) select_acceleration_dialog),
		 (GtkWidget *) vbox);  

  /* copy selection */
  select_acceleration_dialog->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_check_button_set_active(select_acceleration_dialog->copy_selection,
			      TRUE);
  gtk_box_append(vbox,
		 GTK_WIDGET(select_acceleration_dialog->copy_selection));  

  /* automation combo box */
  select_acceleration_dialog->port = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_box_append(vbox,
		 GTK_WIDGET(select_acceleration_dialog->port));  
      
  /* select x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* select x0 - spin button */
  select_acceleration_dialog->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_SELECT_ACCELERATION_MAX_BEATS,
											   0.25);
  gtk_spin_button_set_digits(select_acceleration_dialog->select_x0,
			     2);
  gtk_spin_button_set_value(select_acceleration_dialog->select_x0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(select_acceleration_dialog->select_x0));
  
  /* select x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* select x1 - spin button */
  select_acceleration_dialog->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_SELECT_ACCELERATION_MAX_BEATS,
											   0.25);
  gtk_spin_button_set_digits(select_acceleration_dialog->select_x1,
			     2);
  gtk_spin_button_set_value(select_acceleration_dialog->select_x1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(select_acceleration_dialog->select_x1));

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) select_acceleration_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_select_acceleration_dialog_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  AgsApplicationContext *application_context;
  
  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (select_acceleration_dialog->connectable_flags)) != 0){
    return;
  }

  select_acceleration_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

   /* application context */
  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_signal_connect((GObject *) select_acceleration_dialog, "response",
		   G_CALLBACK(ags_select_acceleration_dialog_response_callback), (gpointer) select_acceleration_dialog);

  /* machine changed */
  g_signal_connect_after((GObject *) window->composite_editor, "machine-changed",
			 G_CALLBACK(ags_select_acceleration_dialog_machine_changed_callback), (gpointer) select_acceleration_dialog);
}

void
ags_select_acceleration_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  AgsApplicationContext *application_context;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (select_acceleration_dialog->connectable_flags)) == 0){
    return;
  }

  select_acceleration_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  g_object_disconnect(G_OBJECT(select_acceleration_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_select_acceleration_dialog_response_callback),
		      select_acceleration_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(window->composite_editor),
		      "any_signal::machine-changed",
		      G_CALLBACK(ags_select_acceleration_dialog_machine_changed_callback),
		      select_acceleration_dialog,
		      NULL);
}

void
ags_select_acceleration_dialog_finalize(GObject *gobject)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = (AgsSelectAccelerationDialog *) gobject;
  
  G_OBJECT_CLASS(ags_select_acceleration_dialog_parent_class)->finalize(gobject);
}

void
ags_select_acceleration_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_acceleration_dialog_apply(AgsApplicable *applicable)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;
  AgsAutomationEdit *focused_automation_edit;

  AgsAudio *audio;

  AgsApplicationContext *application_context;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;

  GList *start_list_automation, *list_automation;
  
  xmlChar *buffer;
  gchar *specifier;
  
  GType channel_type;

  gdouble gui_y;
  
  gdouble c_y0, c_y1;
  gdouble val;
  gdouble upper, lower, range, step;
  gdouble c_upper, c_lower, c_range;

  int size;
  guint x0, y0;
  guint x1, y1;
  guint i;
  gint line;
  
  gboolean copy_selection;
    
  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  focused_automation_edit = (AgsAutomationEdit *) composite_editor->automation_edit->focused_edit;
    
  notebook = composite_editor->automation_edit->channel_selector;
  
  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;

  g_object_get(audio,
	       "automation", &start_list_automation,
	       NULL);

  /* get some values */
  copy_selection = gtk_check_button_get_active(select_acceleration_dialog->copy_selection);

  x0 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x0);

  x1 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x1);
  
  /* select acceleration */
  clipboard = NULL;
  audio_node = NULL;
  
  if(copy_selection){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  
    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
  }

  line = 0;

  channel_type = focused_automation_edit->channel_type;

  specifier = focused_automation_edit->control_name;

  if(notebook != NULL){
    line = ags_notebook_next_active_tab(notebook,
					line);
  }
  
  goto ags_select_acceleration_dialog_apply_LOOP;
    
  while((line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
  ags_select_acceleration_dialog_apply_LOOP:
    
    list_automation = start_list_automation;

    while((list_automation = ags_automation_find_specifier_with_type_and_line(list_automation,
									      specifier,
									      channel_type,
									      line)) != NULL){
      AgsAutomation *current_automation;
      AgsPort *current_port;

      AgsConversion *conversion;
	
      AgsTimestamp *timestamp;
	
      current_automation = list_automation->data;

      g_object_get(current_automation,
		   "timestamp", &timestamp,
		   NULL);

      g_object_unref(timestamp);
	
      if(ags_timestamp_get_ags_offset(timestamp) + AGS_AUTOMATION_DEFAULT_OFFSET < x0){
	list_automation = list_automation->next;
	  
	continue;
      }

      if(ags_timestamp_get_ags_offset(timestamp) > x1){
	break;
      }

      g_object_get(current_automation,
		   "port", &current_port,
		   "upper", &upper,
		   "lower", &lower,
		   NULL);

      g_object_get(current_port,
		   "conversion", &conversion,
		   NULL);
	
      range = upper - lower;

      if(conversion != NULL){
	c_upper = ags_conversion_convert(conversion,
					 upper,
					 FALSE);
	c_lower = ags_conversion_convert(conversion,
					 lower,
					 FALSE);
	c_range = c_upper - c_lower;

	g_object_unref(conversion);
      }else{
	c_upper = upper;
	c_lower = lower;
	  
	c_range = range;
      }

      g_object_unref(current_port);
	
      if(range == 0.0){
	list_automation = list_automation->next;
	g_warning("ags_select_acceleration_dialog.c - range = 0.0");
	  
	continue;
      }
	
      /* check steps */
      g_object_get(current_automation,
		   "steps", &gui_y,
		   NULL);

      val = c_lower + (gui_y * (c_range / gui_y));
      c_y0 = val;

      /* conversion */
      if(conversion != NULL){
	c_y0 = ags_conversion_convert(conversion,
				      c_y0,
				      TRUE);
      }

      /* check steps */
      gui_y = 0;

      val = c_lower + (gui_y * (c_range / gui_y));
      c_y1 = val;

      /* conversion */
      if(conversion != NULL){
	c_y1 = ags_conversion_convert(conversion,
				      c_y1,
				      TRUE);
      }
	    
      /* select */
      ags_automation_add_region_to_selection(current_automation,
					     x0 * AGS_SELECT_ACCELERATION_DEFAULT_WIDTH, c_y0,
					     x1 * AGS_SELECT_ACCELERATION_DEFAULT_WIDTH, c_y1,
					     TRUE);


      if(copy_selection){
	automation_node = ags_automation_copy_selection(list_automation->data);
	xmlAddChild(audio_node, automation_node);      
      }

      list_automation = list_automation->next;
    }

    line++;
  }
  
  g_list_free_full(start_list_automation,
		   (GDestroyNotify) g_object_unref);

  /* write to clipboard */
  if(copy_selection){
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);

    gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			   buffer);
    
    xmlFreeDoc(clipboard);
  }  
}

void
ags_select_acceleration_dialog_reset(AgsApplicable *applicable)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;  
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;

  AgsApplicationContext *application_context;

  GList *start_port, *port;

  gchar **collected_specifier;
  gchar *control_name;
  
  gint position;
  guint length;
  guint i;
  
  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;
  
  machine = composite_editor->selected_machine;
  
  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;
  
  /*  */  
  collected_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  /* audio */
  control_name = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name;

  port =
    start_port = ags_audio_collect_all_audio_ports(audio);

  position = -1;

  i = 0;
  
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

    if(specifier == NULL){
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
       !contains_control_name){
      gtk_combo_box_text_append_text(select_acceleration_dialog->port,
				     g_strdup(specifier));

      if(position == -1 &&
	 !g_strcmp0(control_name, specifier)){
	position = i;
      }
      
      /* add to collected specifier */
      collected_specifier = (gchar **) realloc(collected_specifier,
					       (length + 1) * sizeof(gchar *));
      collected_specifier[length - 1] = g_strdup(specifier);
      collected_specifier[length] = NULL;

      length++;
      i++;
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
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(select_acceleration_dialog->port,
				       g_strdup(specifier));

	if(position == -1 &&
	   !g_strcmp0(control_name, specifier)){
	  position = i;
	}

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
	i++;
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
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(select_acceleration_dialog->port,
				       g_strdup(specifier));

	if(position == -1 &&
	   !g_strcmp0(control_name, specifier)){
	  position = i;
	}

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
	i++;
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

  if(position != -1){
    gtk_combo_box_set_active(select_acceleration_dialog->port,
			     position);
  }
  
  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_strfreev(collected_specifier);
}

/**
 * ags_select_acceleration_dialog_new:
 *
 * Create a new #AgsSelectAccelerationDialog.
 *
 * Returns: a new #AgsSelectAccelerationDialog
 *
 * Since: 3.0.0
 */
AgsSelectAccelerationDialog*
ags_select_acceleration_dialog_new()
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = (AgsSelectAccelerationDialog *) g_object_new(AGS_TYPE_SELECT_ACCELERATION_DIALOG,
									    NULL);

  return(select_acceleration_dialog);
}
