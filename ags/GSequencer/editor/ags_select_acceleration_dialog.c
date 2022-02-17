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

#include <ags/GSequencer/editor/ags_select_acceleration_dialog.h>
#include <ags/GSequencer/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_automation_window.h>
#include <ags/GSequencer/ags_automation_editor.h>
#include <ags/GSequencer/ags_machine.h>

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
  PROP_MAIN_WINDOW,
};

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
   * AgsSelectAccelerationDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 3.0.0
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
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(select_acceleration_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* copy selection */
  select_acceleration_dialog->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_toggle_button_set_active((GtkToggleButton *) select_acceleration_dialog->copy_selection,
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
  select_acceleration_dialog->add = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Add"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_acceleration_dialog->add),
		     FALSE, FALSE,
		     0);  
  
  /* select x0 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
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
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
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
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
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

      select_acceleration_dialog->main_window = (GtkWidget *) main_window;
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
  AgsWindow *window;
  AgsSelectAccelerationDialog *select_acceleration_dialog;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;

  select_acceleration_dialog = AGS_SELECT_ACCELERATION_DIALOG(connectable);

  if((AGS_SELECT_ACCELERATION_DIALOG_CONNECTED & (select_acceleration_dialog->flags)) != 0){
    return;
  }

  select_acceleration_dialog->flags |= AGS_SELECT_ACCELERATION_DIALOG_CONNECTED;

   /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_signal_connect(select_acceleration_dialog, "response",
		   G_CALLBACK(ags_select_acceleration_dialog_response_callback), select_acceleration_dialog);

  /* add automation */
  g_signal_connect(select_acceleration_dialog->add, "clicked",
		   G_CALLBACK(ags_select_acceleration_dialog_add_callback), select_acceleration_dialog);

  /* machine changed */
  if(use_composite_editor){
     AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;
    
    g_signal_connect_after(composite_editor, "machine-changed",
			   G_CALLBACK(ags_select_acceleration_dialog_machine_changed_callback), select_acceleration_dialog);
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;
    
    g_signal_connect_after(automation_editor, "machine-changed",
			   G_CALLBACK(ags_select_acceleration_dialog_machine_changed_callback), select_acceleration_dialog);
  }
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
  AgsMachine *machine;
  AgsNotebook *notebook;
  AgsAutomationEdit *focused_automation_edit;

  AgsAudio *audio;

  AgsApplicationContext *application_context;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;

  GtkTreeModel *model;
  GtkTreeIter iter;

  GList *start_list_automation, *list_automation;
  GList *port, *port_start;
  GList *list;
  
  gchar **specifier;
  xmlChar *buffer;
  gchar *str;
  gchar *scope;
  
  GType channel_type;

  gboolean use_composite_editor;
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

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;

  channel_type = G_TYPE_NONE;
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;

    focused_automation_edit = composite_editor->automation_edit->focused_edit;
    
    notebook = composite_editor->automation_edit->channel_selector;
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;

    machine = automation_editor->selected_machine;

    focused_automation_edit = automation_editor->focused_automation_edit;
    
    if(automation_editor->focused_automation_edit->channel_type == G_TYPE_NONE){
      notebook = NULL;
      channel_type = G_TYPE_NONE;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
      channel_type = AGS_TYPE_OUTPUT;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
      channel_type = AGS_TYPE_INPUT;
    }
  }
  
  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;

  g_object_get(audio,
	       "automation", &start_list_automation,
	       NULL);

  /* get some values */
  copy_selection = gtk_toggle_button_get_active((GtkToggleButton *) select_acceleration_dialog->copy_selection);

  x0 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x0);

  x1 = (AGS_SELECT_ACCELERATION_DEFAULT_WIDTH / 16) * gtk_spin_button_get_value_as_int(select_acceleration_dialog->select_x1);
  
  /* select acceleration */
  port =
    port_start = gtk_container_get_children((GtkContainer *) select_acceleration_dialog->port);
  
  specifier = NULL;

  clipboard = NULL;
  audio_node = NULL;
  
  if(copy_selection){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  
    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
  }

  for(i = 0; port != NULL;){
    list = gtk_container_get_children((GtkContainer *) port->data);

    model = gtk_combo_box_get_model(list->data);
    gtk_tree_model_get(model,
		       &iter,
		       0, &scope,
		       1, &str,
		       -1);

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

    channel_type = G_TYPE_NONE;

    if(!g_strcmp0(scope, "output")){
      channel_type = AGS_TYPE_OUTPUT;
    }else if(!g_strcmp0(scope, "input")){
      channel_type = AGS_TYPE_INPUT;
    }
    
    while((line = ags_notebook_next_active_tab(notebook,
					       line)) != -1){
      list_automation = start_list_automation;

      while((list_automation = ags_automation_find_specifier_with_type_and_line(list_automation,
										specifier[i],
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

    port = port->next;
    i++;
  }

  g_strfreev(specifier);
  
  g_list_free_full(start_list_automation,
		   g_object_unref);
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
    list_start = gtk_container_get_children((GtkContainer *) select_acceleration_dialog->port);

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
 * Since: 3.0.0
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
