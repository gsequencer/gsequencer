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

#include <ags/app/editor/ags_ramp_acceleration_popover.h>
#include <ags/app/editor/ags_ramp_acceleration_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_ramp_acceleration_popover_class_init(AgsRampAccelerationPopoverClass *ramp_acceleration_popover);
void ags_ramp_acceleration_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ramp_acceleration_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_ramp_acceleration_popover_init(AgsRampAccelerationPopover *ramp_acceleration_popover);
void ags_ramp_acceleration_popover_finalize(GObject *gobject);

gboolean ags_ramp_acceleration_popover_is_connected(AgsConnectable *connectable);
void ags_ramp_acceleration_popover_connect(AgsConnectable *connectable);
void ags_ramp_acceleration_popover_disconnect(AgsConnectable *connectable);

void ags_ramp_acceleration_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_ramp_acceleration_popover_apply(AgsApplicable *applicable);
void ags_ramp_acceleration_popover_reset(AgsApplicable *applicable);

gboolean ags_ramp_acceleration_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							    guint keyval,
							    guint keycode,
							    GdkModifierType state,
							    AgsRampAccelerationPopover *ramp_acceleration_popover);
void ags_ramp_acceleration_popover_key_released_callback(GtkEventControllerKey *event_controller,
							 guint keyval,
							 guint keycode,
							 GdkModifierType state,
							 AgsRampAccelerationPopover *ramp_acceleration_popover);
gboolean ags_ramp_acceleration_popover_modifiers_callback(GtkEventControllerKey *event_controller,
							  GdkModifierType keyval,
							  AgsRampAccelerationPopover *ramp_acceleration_popover);

void ags_ramp_acceleration_popover_activate_button_callback(GtkButton *activate_button,
							    AgsRampAccelerationPopover *ramp_acceleration_popover);

/**
 * SECTION:ags_ramp_acceleration_popover
 * @short_description: crop tool
 * @title: AgsRampAccelerationPopover
 * @section_id:
 * @include: ags/app/editor/ags_ramp_acceleration_popover.h
 *
 * The #AgsRampAccelerationPopover lets you crop notes.
 */

static gpointer ags_ramp_acceleration_popover_parent_class = NULL;

GType
ags_ramp_acceleration_popover_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ramp_acceleration_popover = 0;

    static const GTypeInfo ags_ramp_acceleration_popover_info = {
      sizeof (AgsRampAccelerationPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ramp_acceleration_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRampAccelerationPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ramp_acceleration_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_acceleration_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_acceleration_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ramp_acceleration_popover = g_type_register_static(GTK_TYPE_POPOVER,
								"AgsRampAccelerationPopover", &ags_ramp_acceleration_popover_info,
								0);
    
    g_type_add_interface_static(ags_type_ramp_acceleration_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ramp_acceleration_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ramp_acceleration_popover);
  }

  return g_define_type_id__volatile;
}

void
ags_ramp_acceleration_popover_class_init(AgsRampAccelerationPopoverClass *ramp_acceleration_popover)
{
  GObjectClass *gobject;

  ags_ramp_acceleration_popover_parent_class = g_type_class_peek_parent(ramp_acceleration_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) ramp_acceleration_popover;

  gobject->finalize = ags_ramp_acceleration_popover_finalize;
}

void
ags_ramp_acceleration_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_ramp_acceleration_popover_is_connected;  
  connectable->connect = ags_ramp_acceleration_popover_connect;
  connectable->disconnect = ags_ramp_acceleration_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ramp_acceleration_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ramp_acceleration_popover_set_update;
  applicable->apply = ags_ramp_acceleration_popover_apply;
  applicable->reset = ags_ramp_acceleration_popover_reset;
}

void
ags_ramp_acceleration_popover_init(AgsRampAccelerationPopover *ramp_acceleration_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ramp_acceleration_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) ramp_acceleration_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_ramp_acceleration_popover_key_pressed_callback), ramp_acceleration_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_ramp_acceleration_popover_key_released_callback), ramp_acceleration_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_ramp_acceleration_popover_modifiers_callback), ramp_acceleration_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) ramp_acceleration_popover,
			(GtkWidget *) vbox);

  /* automation */
  ramp_acceleration_popover->port = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_append(vbox,
		 GTK_WIDGET(ramp_acceleration_popover->port));  
  
  /* ramp x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x0 - spin button */
  ramp_acceleration_popover->ramp_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											AGS_RAMP_ACCELERATION_MAX_BEATS,
											0.25);
  gtk_spin_button_set_digits(ramp_acceleration_popover->ramp_x0,
			     2);
  gtk_spin_button_set_value(ramp_acceleration_popover->ramp_x0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_acceleration_popover->ramp_x0));

  /* ramp y0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y0 - spin button */
  ramp_acceleration_popover->ramp_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											1.0,
											0.001);
  gtk_spin_button_set_value(ramp_acceleration_popover->ramp_y0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_acceleration_popover->ramp_y0));
  
  /* ramp x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x1 - spin button */
  ramp_acceleration_popover->ramp_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											AGS_RAMP_ACCELERATION_MAX_BEATS,
											0.25);
  gtk_spin_button_set_digits(ramp_acceleration_popover->ramp_x1,
			     2);
  gtk_spin_button_set_value(ramp_acceleration_popover->ramp_x1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_acceleration_popover->ramp_x1));

  /* ramp y1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y1 - spin button */
  ramp_acceleration_popover->ramp_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											1.0,
											0.001);
  gtk_spin_button_set_value(ramp_acceleration_popover->ramp_y1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_acceleration_popover->ramp_y1));

  /* ramp step count - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp step count - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp step count"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp step count - spin button */
  ramp_acceleration_popover->ramp_step_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												AGS_RAMP_ACCELERATION_MAX_STEPS,
												1.0);
  gtk_spin_button_set_value(ramp_acceleration_popover->ramp_step_count,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_acceleration_popover->ramp_step_count));
  
  /* buttons */
  ramp_acceleration_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								  AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) ramp_acceleration_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) ramp_acceleration_popover->action_area);

  ramp_acceleration_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(ramp_acceleration_popover->action_area,
		 (GtkWidget *) ramp_acceleration_popover->activate_button);

  g_signal_connect(ramp_acceleration_popover->activate_button, "clicked",
		   G_CALLBACK(ags_ramp_acceleration_popover_activate_button_callback), ramp_acceleration_popover);

  gtk_popover_set_default_widget((GtkPopover *) ramp_acceleration_popover,
				 (GtkWidget *) ramp_acceleration_popover->activate_button);
}

gboolean
ags_ramp_acceleration_popover_is_connected(AgsConnectable *connectable)
{
  AgsRampAccelerationPopover *ramp_acceleration_popover;
  
  gboolean is_connected;
  
  ramp_acceleration_popover = AGS_RAMP_ACCELERATION_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (ramp_acceleration_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_ramp_acceleration_popover_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampAccelerationPopover *ramp_acceleration_popover;

  AgsApplicationContext *application_context;

  ramp_acceleration_popover = AGS_RAMP_ACCELERATION_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ramp_acceleration_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  g_signal_connect((GObject *) ramp_acceleration_popover->port, "changed",
		   G_CALLBACK(ags_ramp_acceleration_popover_port_callback), (gpointer) ramp_acceleration_popover);


  /* machine changed */
  g_signal_connect_after((GObject *) window->composite_editor, "machine-changed",
			 G_CALLBACK(ags_ramp_acceleration_popover_machine_changed_callback), (gpointer) ramp_acceleration_popover);
}

void
ags_ramp_acceleration_popover_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampAccelerationPopover *ramp_acceleration_popover;

  AgsApplicationContext *application_context;

  ramp_acceleration_popover = AGS_RAMP_ACCELERATION_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ramp_acceleration_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  g_object_disconnect(G_OBJECT(ramp_acceleration_popover->port),
		      "any_signal::changed",
		      G_CALLBACK(ags_ramp_acceleration_popover_port_callback),
		      ramp_acceleration_popover,
		      NULL);

  g_object_disconnect(G_OBJECT(window->composite_editor),
		      "any_signal::machine-changed",
		      G_CALLBACK(ags_ramp_acceleration_popover_machine_changed_callback),
		      ramp_acceleration_popover,
		      NULL);
}

void
ags_ramp_acceleration_popover_finalize(GObject *gobject)
{
  AgsRampAccelerationPopover *ramp_acceleration_popover;

  ramp_acceleration_popover = (AgsRampAccelerationPopover *) gobject;
  
  G_OBJECT_CLASS(ags_ramp_acceleration_popover_parent_class)->finalize(gobject);
}

void
ags_ramp_acceleration_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ramp_acceleration_popover_apply(AgsApplicable *applicable)
{
  AgsRampAccelerationPopover *ramp_acceleration_popover;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;
  AgsAutomationEdit *focused_automation_edit;

  AgsAudio *audio;
  AgsAutomation *current;
  AgsAcceleration *acceleration;

  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GList *start_list_automation, *list_automation;
  GList *list_acceleration, *list_acceleration_start;

  gchar *specifier;
  
  GType channel_type;

  gdouble gui_y;
  gdouble tact;
  
  gdouble c_y0, c_y1;
  gdouble val;
  gdouble upper, lower, range, step;
  gdouble c_upper, c_lower, c_range;

  guint x0, x1, tmp_x;
  gdouble y0, y1, tmp_y;
  guint step_count;
  gint line;
  guint i, i_start, i_stop;
  guint nth_match, match_count;

  gboolean success;
  
  ramp_acceleration_popover = AGS_RAMP_ACCELERATION_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));  
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  focused_automation_edit = (AgsAutomationEdit *) composite_editor->automation_edit->focused_edit;

  if(focused_automation_edit == NULL){
    return;
  }
    
  notebook = composite_editor->automation_edit->channel_selector;

  channel_type = focused_automation_edit->channel_type;
  
  if(machine == NULL){
    return;
  }

  specifier = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name;
  
  audio = machine->audio;

  /* get some values */
  x0 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_acceleration_popover->ramp_x0);
  y0 = gtk_spin_button_get_value(ramp_acceleration_popover->ramp_y0);

  x1 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_acceleration_popover->ramp_x1);
  y1 = gtk_spin_button_get_value(ramp_acceleration_popover->ramp_y1);

  /* make ascending x position */
  if(x0 > x1){
    tmp_x = x0;
    tmp_y = y0;

    x0 = x1;
    y0 = y1;

    x1 = tmp_x;
    y1 = tmp_y;
  }
  
  step_count = gtk_spin_button_get_value_as_int(ramp_acceleration_popover->ramp_step_count);

  /* remove acceleration of region */
  line = 0;

  g_object_get(audio,
	       "automation", &start_list_automation,
	       NULL);

  if(notebook != NULL){
    line = ags_notebook_next_active_tab(notebook,
					line);
  }
  
  goto ags_ramp_acceleration_popover_apply_LOOP_REMOVE;
  
  while(notebook != NULL &&
	(line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
  ags_ramp_acceleration_popover_apply_LOOP_REMOVE:
    
    list_automation = start_list_automation;

    while((list_automation = ags_automation_find_specifier_with_type_and_line(list_automation,
									      specifier,
									      channel_type,
									      line)) != NULL){
      AgsPort *current_port;
      
      AgsTimestamp *current_timestamp;

      AgsConversion *conversion;

      guint steps;
      
      current = AGS_AUTOMATION(list_automation->data);

      current_timestamp = NULL;
      
      g_object_get(current,
		   "timestamp", &current_timestamp,
		   NULL);

      g_object_unref(current_timestamp);
      
      if(ags_timestamp_get_ags_offset(current_timestamp) + AGS_AUTOMATION_DEFAULT_OFFSET < x0){
	list_automation = list_automation->next;
	
	continue;
      }

      if(ags_timestamp_get_ags_offset(current_timestamp) > x1){
	break;
      }

      g_object_get(current,
		   "port", &current_port,
		   "upper", &upper,
		   "lower", &lower,
		   "steps", &steps,
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
      }else{
	c_upper = upper;
	c_lower = lower;
	  
	c_range = range;
      }

      g_object_unref(current_port);
      
      if(range == 0.0){
	list_automation = list_automation->next;
	g_warning("ags_ramp_acceleration_popover.c - range = 0.0");
	  
	continue;
      }
	
      /* check steps */
      gui_y = steps;

      val = c_lower + (gui_y * (c_range / steps));
      c_y0 = val;

      /* conversion */
      if(conversion != NULL){
	c_y0 = ags_conversion_convert(conversion,
				      c_y0,
				      TRUE);
      }

      /* check steps */
      gui_y = 0;

      val = c_lower + (gui_y * (c_range / steps));
      c_y1 = val;

      /* conversion */
      if(conversion != NULL){
	c_y1 = ags_conversion_convert(conversion,
				      c_y1,
				      TRUE);
      }

      /* find and remove region */
      list_acceleration =
	list_acceleration_start = ags_automation_find_region(list_automation->data,
							     x0, c_y0,
							     x1, c_y1,
							     FALSE);
      
      while(list_acceleration != NULL){
	current->acceleration = g_list_remove(current->acceleration,
					      list_acceleration->data);
	
	list_acceleration = list_acceleration->next;
      }

      g_list_free(list_acceleration_start);
      
      list_automation = list_automation->next;
    }

    if(notebook == NULL){
      break;
    }

    line++;
  }

  g_list_free_full(start_list_automation,
		   g_object_unref);
  
  /* ramp acceleration */
  notebook = composite_editor->automation_edit->channel_selector;

  line = 0;

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  if(notebook != NULL){
    line = ags_notebook_next_active_tab(notebook,
					line);
  }
  
  goto ags_ramp_acceleration_popover_apply_LOOP_ADD;  
  
  while(notebook != NULL &&
	(line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
    AgsChannel *start_channel;
    AgsChannel *nth_channel;
    GList *start_play_port, *start_recall_port;
    GList *play_port, *recall_port;

    guint j;
    guint tmp;

  ags_ramp_acceleration_popover_apply_LOOP_ADD:
    
    play_port =
      start_play_port = NULL;
    recall_port =
      start_recall_port = NULL;
    
    if(channel_type == AGS_TYPE_OUTPUT){
      g_object_get(machine->audio,
		   "output", &start_channel,
		   NULL);
      
      nth_channel = ags_channel_nth(start_channel,
				    line);

      play_port =
	start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											 specifier, TRUE);

      recall_port = 
	start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   specifier, FALSE);

      if(nth_channel != NULL){
	g_object_unref(nth_channel);
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
    }else if(channel_type == AGS_TYPE_INPUT){
      g_object_get(machine->audio,
		   "input", &start_channel,
		   NULL);

      nth_channel = ags_channel_nth(start_channel,
				    line);

      play_port =
	start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											 specifier, TRUE);

      recall_port = 
	start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   specifier, FALSE);

      if(nth_channel != NULL){
	g_object_unref(nth_channel);
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
    }else{
      play_port =
	start_play_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										     specifier, TRUE);
      
      recall_port =
	start_recall_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										       specifier, FALSE);
    }
        
    if(play_port != NULL){
      match_count = 0;
      tmp = x1 - x0;

      if(x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET != 0){
	match_count = 1;

	if(tmp >= AGS_AUTOMATION_DEFAULT_OFFSET){
	  match_count += (guint) ceil((gdouble) (tmp - (x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET)) / AGS_AUTOMATION_DEFAULT_OFFSET);
	}
      }else{
	match_count += (guint) ceil((gdouble) tmp / AGS_AUTOMATION_DEFAULT_OFFSET);
      }
      
      success = FALSE;

      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      i = 0;

      for(nth_match = 0; nth_match < match_count; nth_match++){
	timestamp->timer.ags_offset.offset = (guint64) floor((double) x0 / (double) AGS_AUTOMATION_DEFAULT_OFFSET);

	if(nth_match > 0){
	  timestamp->timer.ags_offset.offset += (nth_match * AGS_AUTOMATION_DEFAULT_OFFSET);
	}
	
	start_list_automation = g_list_copy(AGS_PORT(play_port->data)->automation);      
	list_automation = ags_automation_find_near_timestamp_extended(start_list_automation, line,
								      AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								      timestamp);

	if(list_automation == NULL){
	  current = ags_automation_new((GObject *) machine->audio,
				       line,
				       channel_type, specifier);

	  current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	  ags_audio_add_automation(machine->audio,
				   (GObject *) current);
	  
	  ags_port_add_automation(play_port->data,
				  (GObject *) current);

	  ags_port_add_automation(recall_port->data,
				  (GObject *) current);
	}else{
	  current = list_automation->data;
	}

	g_list_free(start_list_automation);

	list_automation =
	  start_list_automation = NULL;
	
	upper = current->upper;
	lower = current->lower;
	
	range = upper - lower;
	
	if(range == 0.0){
	  g_warning("ags_ramp_acceleration_popover.c - range = 0.0");
	  
	  break;
	}
	    
	/* ramp value and move offset */
	if(!success){
	  if(floor(x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET)){
	    i_stop = step_count;

	    tmp = 0;
	  }else{
	    i_stop = (x1 - tmp) / (x1 - x0) * step_count;

	    if(tmp - AGS_AUTOMATION_DEFAULT_OFFSET >= 0){
	      tmp -= AGS_AUTOMATION_DEFAULT_OFFSET;
	    }else{
	      tmp = 0;
	    }
	  }
	
	  success = TRUE;
	}else{
	  i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
	  if(tmp - AGS_AUTOMATION_DEFAULT_OFFSET >= 0){
	    tmp -= AGS_AUTOMATION_DEFAULT_OFFSET;
	  }else{
	    tmp = 0;
	  }
	}
	
	for(; i < step_count && i < i_stop; i++){
	  acceleration = ags_acceleration_new();
	  acceleration->x = ((gdouble) x0 + (gdouble) ((gdouble) (x1 - x0) * (gdouble) ((gdouble) (i) / ((gdouble) step_count))));
	  acceleration->y = ((gdouble) y0 + ((gdouble) (y1 - y0) * (gdouble) ((gdouble) (i + 1) / ((gdouble) step_count))));

#ifdef AGS_DEBUG
	  g_message("line %d - %d %f", line, acceleration->x, acceleration->y);
#endif
	
	  ags_automation_add_acceleration(current,
					  acceleration,
					  FALSE);
	
	}	
      }
    }

    if(recall_port != NULL){
      match_count = 0;
      tmp = x1 - x0;

      if(x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET != 0){
	match_count = 1;

	if(tmp >= AGS_AUTOMATION_DEFAULT_OFFSET){
	  match_count += (guint) ceil((gdouble) (tmp - (x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET)) / AGS_AUTOMATION_DEFAULT_OFFSET);
	}
      }else{
	match_count += (guint) ceil((gdouble) tmp / AGS_AUTOMATION_DEFAULT_OFFSET);
      }
      
      success = FALSE;

      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      i = 0;

      for(nth_match = 0; nth_match < match_count; nth_match++){
	timestamp->timer.ags_offset.offset = (guint64) floor((double) x0 / (double) AGS_AUTOMATION_DEFAULT_OFFSET);

	if(nth_match > 0){
	  timestamp->timer.ags_offset.offset += (nth_match * AGS_AUTOMATION_DEFAULT_OFFSET);
	}
	
	start_list_automation = g_list_copy(AGS_PORT(recall_port->data)->automation);
	list_automation = ags_automation_find_near_timestamp_extended(start_list_automation, line,
								      AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								      timestamp);

	if(list_automation == NULL){
	  current = ags_automation_new((GObject *) machine->audio,
				       line,
				       channel_type, specifier);

	  current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	  ags_audio_add_automation(machine->audio,
				   (GObject *) current);

	  ags_port_add_automation(play_port->data,
				  (GObject *) current);
	  ags_port_add_automation(recall_port->data,
				  (GObject *) current);
	}else{
	  current = list_automation->data;
	}

	g_list_free(start_list_automation);

	list_automation =
	  start_list_automation = NULL;

	upper = current->upper;
	lower = current->lower;
	
	range = upper - lower;
	
	if(range == 0.0){
	  g_warning("ags_ramp_acceleration_popover.c - range = 0.0");
	  
	  break;
	}
	    
	/* ramp value and move offset */
	if(!success){
	  if(floor(x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET)){
	    i_stop = step_count;

	    tmp = 0;
	  }else{
	    i_stop = (x1 - tmp) / (x1 - x0) * step_count;

	    if(tmp - AGS_AUTOMATION_DEFAULT_OFFSET >= 0){
	      tmp -= AGS_AUTOMATION_DEFAULT_OFFSET;
	    }else{
	      tmp = 0;
	    }
	  }
	
	  success = TRUE;
	}else{
	  i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
	  if(tmp - AGS_AUTOMATION_DEFAULT_OFFSET >= 0){
	    tmp -= AGS_AUTOMATION_DEFAULT_OFFSET;
	  }else{
	    tmp = 0;
	  }
	}
	
	for(; i < step_count && i < i_stop; i++){
	  acceleration = ags_acceleration_new();
	  acceleration->x = ((gdouble) x0 + (gdouble) ((gdouble) (x1 - x0) * (gdouble) ((gdouble) (i) / ((gdouble) step_count))));
	  acceleration->y = ((gdouble) y0 + ((gdouble) (y1 - y0) * (gdouble) ((gdouble) (i + 1) / ((gdouble) step_count))));

#ifdef AGS_DEBUG
	  g_message("line %d - %d %f", line, acceleration->x, acceleration->y);
#endif
	
	  ags_automation_add_acceleration(current,
					  acceleration,
					  FALSE);
	
	}
      }
    }

    g_list_free_full(start_play_port,
		     g_object_unref);
    
    g_list_free_full(start_recall_port,
		     g_object_unref);

    if(notebook == NULL){
      break;
    }
    
    line++;
  }

  gtk_widget_queue_draw((GtkWidget *) AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->drawing_area);
}

void
ags_ramp_acceleration_popover_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsRampAccelerationPopover *ramp_acceleration_popover;

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

  ramp_acceleration_popover = AGS_RAMP_ACCELERATION_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  if(window == NULL ||
     window->composite_editor == NULL){
    return;
  }
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ramp_acceleration_popover->port))));
  
  if(machine == NULL ||
     composite_editor->automation_edit == NULL ||
     composite_editor->automation_edit->focused_edit == NULL){
    return;
  }
  
  audio = machine->audio;
  
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
      gtk_combo_box_text_append_text(ramp_acceleration_popover->port,
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
	gtk_combo_box_text_append_text(ramp_acceleration_popover->port,
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
	gtk_combo_box_text_append_text(ramp_acceleration_popover->port,
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
    gtk_combo_box_set_active((GtkComboBox *) ramp_acceleration_popover->port,
			     position);
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_strfreev(collected_specifier);
}

gboolean
ags_ramp_acceleration_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsRampAccelerationPopover *ramp_acceleration_popover)
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
ags_ramp_acceleration_popover_key_released_callback(GtkEventControllerKey *event_controller,
						    guint keyval,
						    guint keycode,
						    GdkModifierType state,
						    AgsRampAccelerationPopover *ramp_acceleration_popover)
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
	gtk_popover_popdown((GtkPopover *) ramp_acceleration_popover);	
      }
      break;
    }
  }
}

gboolean
ags_ramp_acceleration_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						 GdkModifierType keyval,
						 AgsRampAccelerationPopover *ramp_acceleration_popover)
{
  return(FALSE);
}

void
ags_ramp_acceleration_popover_activate_button_callback(GtkButton *activate_button,
						       AgsRampAccelerationPopover *ramp_acceleration_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(ramp_acceleration_popover));
}

/**
 * ags_ramp_acceleration_popover_new:
 *
 * Create a new #AgsRampAccelerationPopover.
 *
 * Returns: a new #AgsRampAccelerationPopover
 *
 * Since: 6.11.0
 */
AgsRampAccelerationPopover*
ags_ramp_acceleration_popover_new()
{
  AgsRampAccelerationPopover *ramp_acceleration_popover;

  ramp_acceleration_popover = (AgsRampAccelerationPopover *) g_object_new(AGS_TYPE_RAMP_ACCELERATION_POPOVER,
									  NULL);

  return(ramp_acceleration_popover);
}
