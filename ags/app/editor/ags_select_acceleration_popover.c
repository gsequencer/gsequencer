/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_select_acceleration_popover.h>
#include <ags/app/editor/ags_select_acceleration_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_select_acceleration_popover_class_init(AgsSelectAccelerationPopoverClass *select_acceleration_popover);
void ags_select_acceleration_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_acceleration_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_acceleration_popover_init(AgsSelectAccelerationPopover *select_acceleration_popover);
void ags_select_acceleration_popover_finalize(GObject *gobject);

gboolean ags_select_acceleration_popover_is_connected(AgsConnectable *connectable);
void ags_select_acceleration_popover_connect(AgsConnectable *connectable);
void ags_select_acceleration_popover_disconnect(AgsConnectable *connectable);

void ags_select_acceleration_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_acceleration_popover_apply(AgsApplicable *applicable);
void ags_select_acceleration_popover_reset(AgsApplicable *applicable);

gboolean ags_select_acceleration_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							      guint keyval,
							      guint keycode,
							      GdkModifierType state,
							      AgsSelectAccelerationPopover *select_acceleration_popover);
void ags_select_acceleration_popover_key_released_callback(GtkEventControllerKey *event_controller,
							   guint keyval,
							   guint keycode,
							   GdkModifierType state,
							   AgsSelectAccelerationPopover *select_acceleration_popover);
gboolean ags_select_acceleration_popover_modifiers_callback(GtkEventControllerKey *event_controller,
							    GdkModifierType keyval,
							    AgsSelectAccelerationPopover *select_acceleration_popover);

void ags_select_acceleration_popover_activate_button_callback(GtkButton *activate_button,
							      AgsSelectAccelerationPopover *select_acceleration_popover);

/**
 * SECTION:ags_select_acceleration_popover
 * @short_description: crop tool
 * @title: AgsSelectAccelerationPopover
 * @section_id:
 * @include: ags/app/editor/ags_select_acceleration_popover.h
 *
 * The #AgsSelectAccelerationPopover lets you crop notes.
 */

static gpointer ags_select_acceleration_popover_parent_class = NULL;

GType
ags_select_acceleration_popover_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_select_acceleration_popover = 0;

    static const GTypeInfo ags_select_acceleration_popover_info = {
      sizeof (AgsSelectAccelerationPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_acceleration_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectAccelerationPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_acceleration_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_acceleration_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_acceleration_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_acceleration_popover = g_type_register_static(GTK_TYPE_POPOVER,
								  "AgsSelectAccelerationPopover", &ags_select_acceleration_popover_info,
								  0);
    
    g_type_add_interface_static(ags_type_select_acceleration_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_acceleration_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_select_acceleration_popover);
  }

  return g_define_type_id__volatile;
}

void
ags_select_acceleration_popover_class_init(AgsSelectAccelerationPopoverClass *select_acceleration_popover)
{
  GObjectClass *gobject;

  ags_select_acceleration_popover_parent_class = g_type_class_peek_parent(select_acceleration_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) select_acceleration_popover;

  gobject->finalize = ags_select_acceleration_popover_finalize;
}

void
ags_select_acceleration_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_select_acceleration_popover_is_connected;  
  connectable->connect = ags_select_acceleration_popover_connect;
  connectable->disconnect = ags_select_acceleration_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_select_acceleration_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_acceleration_popover_set_update;
  applicable->apply = ags_select_acceleration_popover_apply;
  applicable->reset = ags_select_acceleration_popover_reset;
}

void
ags_select_acceleration_popover_init(AgsSelectAccelerationPopover *select_acceleration_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  select_acceleration_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) select_acceleration_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_select_acceleration_popover_key_pressed_callback), select_acceleration_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_select_acceleration_popover_key_released_callback), select_acceleration_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_select_acceleration_popover_modifiers_callback), select_acceleration_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) select_acceleration_popover,
			(GtkWidget *) vbox);

  /* copy selection */
  select_acceleration_popover->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_check_button_set_active(select_acceleration_popover->copy_selection,
			      TRUE);
  gtk_box_append(vbox,
		 GTK_WIDGET(select_acceleration_popover->copy_selection));  

  /* automation combo box */
  select_acceleration_popover->port = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_box_append(vbox,
		 GTK_WIDGET(select_acceleration_popover->port));  
      
  /* select x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* select x0 - spin button */
  select_acceleration_popover->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											    AGS_SELECT_ACCELERATION_MAX_BEATS,
											    0.25);
  gtk_spin_button_set_digits(select_acceleration_popover->select_x0,
			     2);
  gtk_spin_button_set_value(select_acceleration_popover->select_x0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(select_acceleration_popover->select_x0));
  
  /* select x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* select x1 - spin button */
  select_acceleration_popover->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											    AGS_SELECT_ACCELERATION_MAX_BEATS,
											    0.25);
  gtk_spin_button_set_digits(select_acceleration_popover->select_x1,
			     2);
  gtk_spin_button_set_value(select_acceleration_popover->select_x1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(select_acceleration_popover->select_x1));
  
  /* buttons */
  select_acceleration_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) select_acceleration_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) select_acceleration_popover->action_area);

  select_acceleration_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(select_acceleration_popover->action_area,
		 (GtkWidget *) select_acceleration_popover->activate_button);

  g_signal_connect(select_acceleration_popover->activate_button, "clicked",
		   G_CALLBACK(ags_select_acceleration_popover_activate_button_callback), select_acceleration_popover);

  gtk_popover_set_default_widget((GtkPopover *) select_acceleration_popover,
				 (GtkWidget *) select_acceleration_popover->activate_button);
}

gboolean
ags_select_acceleration_popover_is_connected(AgsConnectable *connectable)
{
  AgsSelectAccelerationPopover *select_acceleration_popover;
  
  gboolean is_connected;
  
  select_acceleration_popover = AGS_SELECT_ACCELERATION_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (select_acceleration_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_select_acceleration_popover_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsSelectAccelerationPopover *select_acceleration_popover;

  AgsApplicationContext *application_context;

  select_acceleration_popover = AGS_SELECT_ACCELERATION_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  select_acceleration_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* machine changed */
  g_signal_connect_after((GObject *) window->composite_editor, "machine-changed",
			 G_CALLBACK(ags_select_acceleration_popover_machine_changed_callback), (gpointer) select_acceleration_popover);
}

void
ags_select_acceleration_popover_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsSelectAccelerationPopover *select_acceleration_popover;

  AgsApplicationContext *application_context;

  select_acceleration_popover = AGS_SELECT_ACCELERATION_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  select_acceleration_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  g_object_disconnect(G_OBJECT(window->composite_editor),
		      "any_signal::machine-changed",
		      G_CALLBACK(ags_select_acceleration_popover_machine_changed_callback),
		      select_acceleration_popover,
		      NULL);
}

void
ags_select_acceleration_popover_finalize(GObject *gobject)
{
  AgsSelectAccelerationPopover *select_acceleration_popover;

  select_acceleration_popover = (AgsSelectAccelerationPopover *) gobject;
  
  G_OBJECT_CLASS(ags_select_acceleration_popover_parent_class)->finalize(gobject);
}

void
ags_select_acceleration_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_acceleration_popover_apply(AgsApplicable *applicable)
{
  AgsSelectAccelerationPopover *select_acceleration_popover;
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
  
  gdouble val;
  gdouble upper, lower, range, step;
  gdouble c_upper, c_lower, c_range;

  int size;
  guint x0, x1;
  guint i;
  gint line;
  
  gboolean copy_selection;
    
  select_acceleration_popover = AGS_SELECT_ACCELERATION_POPOVER(applicable);

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

  start_list_automation = NULL;
  
  g_object_get(audio,
	       "automation", &start_list_automation,
	       NULL);

  /* get some values */
  copy_selection = gtk_check_button_get_active(select_acceleration_popover->copy_selection);

  x0 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(select_acceleration_popover->select_x0);

  x1 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(select_acceleration_popover->select_x1);
  
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
  
  goto ags_select_acceleration_popover_apply_LOOP;
    
  while((line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
  ags_select_acceleration_popover_apply_LOOP:
    
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

      timestamp = NULL;
      
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
	g_warning("ags_select_acceleration_popover.c - range = 0.0");
	  
	continue;
      }
		    
      /* select */
      ags_automation_add_region_to_selection(current_automation,
					     x0, lower,
					     x1, upper,
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
ags_select_acceleration_popover_reset(AgsApplicable *applicable)
{
  AgsSelectAccelerationPopover *select_acceleration_popover;  
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
  
  select_acceleration_popover = AGS_SELECT_ACCELERATION_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  if(window == NULL ||
     window->composite_editor == NULL){
    return;
  }

  composite_editor = window->composite_editor;
  
  machine = composite_editor->selected_machine;
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(select_acceleration_popover->port))));

  if(machine == NULL ||
     composite_editor->automation_edit == NULL ||
     composite_editor->automation_edit->focused_edit == NULL){
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
    contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){
      gtk_combo_box_text_append_text(select_acceleration_popover->port,
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
    
    /* iterate */
    port = port->next;
  }

  g_list_free_full(start_port,
		   g_object_unref);
    
  /* output */
  start_channel = NULL;
  
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
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
      contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(select_acceleration_popover->port,
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

  if(channel != NULL){
    g_object_ref(channel);
  }

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
      contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(select_acceleration_popover->port,
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
    gtk_combo_box_set_active((GtkComboBox *) select_acceleration_popover->port,
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

gboolean
ags_select_acceleration_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsSelectAccelerationPopover *select_acceleration_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_select_acceleration_popover_key_released_callback(GtkEventControllerKey *event_controller,
						      guint keyval,
						      guint keycode,
						      GdkModifierType state,
						      AgsSelectAccelerationPopover *select_acceleration_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_popover_popdown((GtkPopover *) select_acceleration_popover);	
      }
      break;
    }
  }
}

gboolean
ags_select_acceleration_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						   GdkModifierType keyval,
						   AgsSelectAccelerationPopover *select_acceleration_popover)
{
  return(FALSE);
}

void
ags_select_acceleration_popover_activate_button_callback(GtkButton *activate_button,
							 AgsSelectAccelerationPopover *select_acceleration_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(select_acceleration_popover));
}

/**
 * ags_select_acceleration_popover_new:
 *
 * Create a new #AgsSelectAccelerationPopover.
 *
 * Returns: a new #AgsSelectAccelerationPopover
 *
 * Since: 6.11.0
 */
AgsSelectAccelerationPopover*
ags_select_acceleration_popover_new()
{
  AgsSelectAccelerationPopover *select_acceleration_popover;

  select_acceleration_popover = (AgsSelectAccelerationPopover *) g_object_new(AGS_TYPE_SELECT_ACCELERATION_POPOVER,
									      NULL);

  return(select_acceleration_popover);
}
