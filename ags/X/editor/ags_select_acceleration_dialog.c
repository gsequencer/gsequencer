/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_select_acceleration_dialog.h>
#include <ags/X/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/i18n.h>

void ags_select_acceleration_dialog_class_init(AgsSelectAccelerationDialogClass *select_acceleration_dialog);
void ags_select_acceleration_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_acceleration_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_acceleration_dialog_init(AgsSelectAccelerationDialog *select_acceleration_dialog);
void ags_select_acceleration_dialog_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_select_acceleration_dialog_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_select_acceleration_dialog_finalize(GObject *gobject);
void ags_select_acceleration_dialog_connect(AgsConnectable *connectable);
void ags_select_acceleration_dialog_disconnect(AgsConnectable *connectable);
void ags_select_acceleration_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_acceleration_dialog_apply(AgsApplicable *applicable);
void ags_select_acceleration_dialog_reset(AgsApplicable *applicable);
gboolean ags_select_acceleration_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_select_acceleration_dialog
 * @short_description: select tool
 * @title: AgsSelectAccelerationDialog
 * @section_id:
 * @include: ags/X/editor/ags_select_acceleration_dialog.h
 *
 * The #AgsSelectAccelerationDialog lets you select accelerations.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_select_acceleration_dialog_parent_class = NULL;

GType
ags_select_acceleration_dialog_get_type(void)
{
  static GType ags_type_select_acceleration_dialog = 0;

  if (!ags_type_select_acceleration_dialog){
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
  }

  return (ags_type_select_acceleration_dialog);
}

void
ags_select_acceleration_dialog_class_init(AgsSelectAccelerationDialogClass *select_acceleration_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_select_acceleration_dialog_parent_class = g_type_class_peek_parent(select_acceleration_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) select_acceleration_dialog;

  gobject->set_property = ags_select_acceleration_dialog_set_property;
  gobject->get_property = ags_select_acceleration_dialog_get_property;

  gobject->finalize = ags_select_acceleration_dialog_finalize;

  /* properties */
  /**
   * AgsSelectAccelerationDialog:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("assigned application context"),
				   i18n_pspec("The AgsApplicationContext it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsSelectAccelerationDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("main-window",
				   i18n_pspec("assigned main window"),
				   i18n_pspec("The assigned main window"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_WINDOW,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) select_acceleration_dialog;

  widget->delete_event = ags_select_acceleration_dialog_delete_event;
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
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  select_acceleration_dialog->flags = 0;

  g_object_set(select_acceleration_dialog,
	       "title", i18n("select accelerations"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) select_acceleration_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* copy selection */
  select_acceleration_dialog->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_toggle_button_set_active(select_acceleration_dialog->copy_selection,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_acceleration_dialog->copy_selection),
		     FALSE, FALSE,
		     0);  

  /* automation */
  select_acceleration_dialog->port = (GtkVBox *) gtk_vbox_new(FALSE,
							      0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_acceleration_dialog->port),
		     FALSE, FALSE,
		     0);  
  
  /* add */
  select_acceleration_dialog->add = (GtkCheckButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_acceleration_dialog->add),
		     FALSE, FALSE,
		     0);  
  
  /* select x0 - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select x0 - spin button */
  select_acceleration_dialog->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_SELECT_ACCELERATION_MAX_BEATS,
											   0.25);
  gtk_spin_button_set_digits(select_acceleration_dialog->select_x0,
			     2);
  gtk_spin_button_set_value(select_acceleration_dialog->select_x0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_acceleration_dialog->select_x0),
		     FALSE, FALSE,
		     0);
  
  /* select x1 - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select x1 - spin button */
  select_acceleration_dialog->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_SELECT_ACCELERATION_MAX_BEATS,
											   0.25);
  gtk_spin_button_set_digits(select_acceleration_dialog->select_x1,
			     2);
  gtk_spin_button_set_value(select_acceleration_dialog->select_x1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_acceleration_dialog->select_x1),
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) select_acceleration_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_select_acceleration_dialog_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) select_acceleration_dialog->application_context == application_context){
	return;
      }
      
      if(select_acceleration_dialog->application_context != NULL){
	g_object_unref(select_acceleration_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      select_acceleration_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) select_acceleration_dialog->main_window == main_window){
	return;
      }

      if(select_acceleration_dialog->main_window != NULL){
	g_object_unref(select_acceleration_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      select_acceleration_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_acceleration_dialog_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, select_acceleration_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, select_acceleration_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_acceleration_dialog_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(connectable);

  if((AGS_SELECT_ACCELERATION_DIALOG_CONNECTED & (select_acceleration_dialog->flags)) != 0){
    return;
  }

  select_acceleration_dialog->flags |= AGS_SELECT_ACCELERATION_DIALOG_CONNECTED;

  automation_editor = AGS_WINDOW(select_acceleration_dialog->main_window)->automation_window->automation_editor;

  g_signal_connect(select_acceleration_dialog, "response",
		   G_CALLBACK(ags_select_acceleration_dialog_response_callback), select_acceleration_dialog);

  /* add automation */
  g_signal_connect(select_acceleration_dialog->add, "clicked",
		   G_CALLBACK(ags_select_acceleration_dialog_add_callback), select_acceleration_dialog);

  /* machine changed */
  g_signal_connect_after(automation_editor, "machine-changed",
			 G_CALLBACK(ags_select_acceleration_dialog_machine_changed_callback), select_acceleration_dialog);  
}

void
ags_select_acceleration_dialog_disconnect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(connectable);

  if((AGS_SELECT_ACCELERATION_DIALOG_CONNECTED & (select_acceleration_dialog->flags)) == 0){
    return;
  }

  select_acceleration_dialog->flags &= (~AGS_SELECT_ACCELERATION_DIALOG_CONNECTED);

  automation_editor = AGS_WINDOW(select_acceleration_dialog->main_window)->automation_window->automation_editor;

  g_object_disconnect(G_OBJECT(select_acceleration_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_select_acceleration_dialog_response_callback),
		      select_acceleration_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(select_acceleration_dialog->add),
		      "any_signal::clicked",
		      G_CALLBACK(ags_select_acceleration_dialog_add_callback),
		      select_acceleration_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_editor),
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

  if(select_acceleration_dialog->application_context != NULL){
    g_object_unref(select_acceleration_dialog->application_context);
  }
  
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
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;

  GList *list_automation;
  GList *port, *port_start;
  GList *list;
  
  gchar **specifier;
  xmlChar *buffer;
  gchar *str;
  
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
  gboolean is_audio, is_output, is_input;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(applicable);

  window = select_acceleration_dialog->main_window;
  automation_editor = window->automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(gtk_notebook_get_current_page(automation_editor->notebook) == 0){
    notebook = NULL;
    channel_type = G_TYPE_NONE;

    is_audio = TRUE;
  }else if(gtk_notebook_get_current_page(automation_editor->notebook) == 1){
    notebook = automation_editor->current_output_notebook;
    channel_type = AGS_TYPE_OUTPUT;
      
    is_output = TRUE;
  }else if(gtk_notebook_get_current_page(automation_editor->notebook) == 2){
    notebook = automation_editor->current_input_notebook;
    channel_type = AGS_TYPE_INPUT;
      
    is_input = TRUE;
  }
  
  audio = machine->audio;

  /* get some values */
  copy_selection = gtk_toggle_button_get_active(select_acceleration_dialog->copy_selection);

  x0 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x0);

  x1 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x1);
  
  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* select acceleration */
  port =
    port_start = gtk_container_get_children(select_acceleration_dialog->port);
  
  specifier = NULL;

  if(copy_selection){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  
    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
  }

  for(i = 0; port != NULL;){
    list = gtk_container_get_children(port->data);
    str = gtk_combo_box_text_get_active_text(list->data);

    g_list_free(list);
    
    if(specifier != NULL &&
       g_strv_contains(specifier,
		       str)){
      port = port->next;

      continue;
    }

    if(specifier == NULL){
      specifier = (gchar **) malloc(2 * sizeof(gchar *));
    }else{
      specifier = (gchar **) realloc(specifier,
				     (i + 2) * sizeof(gchar *));
    }
    
    specifier[i] = str;
    specifier[i + 1] = NULL;
    
    line = 0;
    
    while((line = ags_notebook_next_active_tab(notebook,
					       line)) != -1){
      list_automation = audio->automation;

      while((list_automation = ags_automation_find_specifier_with_type_and_line(list_automation,
										specifier[i],
										channel_type,
										line)) != NULL){

	upper = AGS_AUTOMATION(list_automation->data)->upper;
	lower = AGS_AUTOMATION(list_automation->data)->lower;
	
	range = upper - lower;

	if(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion != NULL){
	  c_upper = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion,
					   upper,
					   FALSE);
	  c_lower = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion,
					   lower,
					   FALSE);
	  c_range = c_upper - c_lower;
	}else{
	  c_upper = upper;
	  c_lower = lower;
	  
	  c_range = range;
	}
	
	if(range == 0.0){
	  list_automation = list_automation->next;
	  g_warning("ags_select_acceleration_dialog.c - range = 0.0");
	  
	  continue;
	}
	
	/* check steps */
	gui_y = AGS_AUTOMATION(list_automation->data)->steps;

	val = c_lower + (gui_y * (c_range / AGS_AUTOMATION(list_automation->data)->steps));
	c_y0 = val;

	/* conversion */
	if(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion != NULL){
	  c_y0 = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion,
					c_y0,
					TRUE);
	}

	/* check steps */
	gui_y = 0;

	val = c_lower + (gui_y * (c_range / AGS_AUTOMATION(list_automation->data)->steps));
	c_y1 = val;

	/* conversion */
	if(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion != NULL){
	  c_y1 = ags_conversion_convert(AGS_PORT(AGS_AUTOMATION(list_automation->data)->port)->conversion,
					c_y1,
					TRUE);
	}
	    
	/* select */
	ags_automation_add_region_to_selection(list_automation->data,
					       x0 * AGS_SELECT_ACCELERATION_DEFAULT_WIDTH, c_y0,
					       x1 * AGS_SELECT_ACCELERATION_DEFAULT_WIDTH, c_y1,
					       TRUE);


	if(copy_selection){
	  automation_node = ags_automation_copy_selection(list_automation->data);
	  ags_automation_merge_clipboard(audio_node,
					 automation_node);
	  xmlFreeNode(automation_node);
	}

	list_automation = list_automation->next;
      }

      line++;
    }

    port = port->next;
    i++;
  }

  g_list_free(port_start);

  /* write to clipboard */
  if(copy_selection){
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    xmlFreeDoc(clipboard);
  }  
}

void
ags_select_acceleration_dialog_reset(AgsApplicable *applicable)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  GList *list_start, *list;
  
  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(applicable);

  list =
    list_start = gtk_container_get_children(select_acceleration_dialog->port);

  while(list != NULL){
    gtk_widget_destroy(list->data);

    list = list->next;
  }

  g_list_free(list_start);
}

gboolean
ags_select_acceleration_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_select_acceleration_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_select_acceleration_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsSelectAccelerationDialog.
 *
 * Returns: a new #AgsSelectAccelerationDialog
 *
 * Since: 1.0.0
 */
AgsSelectAccelerationDialog*
ags_select_acceleration_dialog_new(GtkWidget *main_window)
{
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  select_acceleration_dialog = (AgsSelectAccelerationDialog *) g_object_new(AGS_TYPE_SELECT_ACCELERATION_DIALOG,
									    "main-window", main_window,
									    NULL);

  return(select_acceleration_dialog);
}
