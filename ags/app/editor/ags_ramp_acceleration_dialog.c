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

#include <ags/app/editor/ags_ramp_acceleration_dialog.h>
#include <ags/app/editor/ags_ramp_acceleration_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_automation_window.h>
#include <ags/app/ags_automation_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <ags/i18n.h>

void ags_ramp_acceleration_dialog_class_init(AgsRampAccelerationDialogClass *ramp_acceleration_dialog);
void ags_ramp_acceleration_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ramp_acceleration_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_ramp_acceleration_dialog_init(AgsRampAccelerationDialog *ramp_acceleration_dialog);
void ags_ramp_acceleration_dialog_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_ramp_acceleration_dialog_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_ramp_acceleration_dialog_finalize(GObject *gobject);
void ags_ramp_acceleration_dialog_connect(AgsConnectable *connectable);
void ags_ramp_acceleration_dialog_disconnect(AgsConnectable *connectable);
void ags_ramp_acceleration_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_ramp_acceleration_dialog_apply(AgsApplicable *applicable);
void ags_ramp_acceleration_dialog_reset(AgsApplicable *applicable);
gboolean ags_ramp_acceleration_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_ramp_acceleration_dialog
 * @short_description: ramp tool
 * @title: AgsRampAccelerationDialog
 * @section_id:
 * @include: ags/app/editor/ags_ramp_acceleration_dialog.h
 *
 * The #AgsRampAccelerationDialog lets you ramp accelerations.
 */

enum{
  PROP_0,
  PROP_MAIN_WINDOW,
};

static gpointer ags_ramp_acceleration_dialog_parent_class = NULL;

GType
ags_ramp_acceleration_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ramp_acceleration_dialog = 0;

    static const GTypeInfo ags_ramp_acceleration_dialog_info = {
      sizeof (AgsRampAccelerationDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ramp_acceleration_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRampAccelerationDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ramp_acceleration_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_acceleration_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_acceleration_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ramp_acceleration_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							       "AgsRampAccelerationDialog", &ags_ramp_acceleration_dialog_info,
							       0);
    
    g_type_add_interface_static(ags_type_ramp_acceleration_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ramp_acceleration_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ramp_acceleration_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_ramp_acceleration_dialog_class_init(AgsRampAccelerationDialogClass *ramp_acceleration_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_ramp_acceleration_dialog_parent_class = g_type_class_peek_parent(ramp_acceleration_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) ramp_acceleration_dialog;

  gobject->set_property = ags_ramp_acceleration_dialog_set_property;
  gobject->get_property = ags_ramp_acceleration_dialog_get_property;

  gobject->finalize = ags_ramp_acceleration_dialog_finalize;

  /* properties */
  /**
   * AgsRampAccelerationDialog:main-window:
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
  widget = (GtkWidgetClass *) ramp_acceleration_dialog;

  widget->delete_event = ags_ramp_acceleration_dialog_delete_event;
}

void
ags_ramp_acceleration_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ramp_acceleration_dialog_connect;
  connectable->disconnect = ags_ramp_acceleration_dialog_disconnect;
}

void
ags_ramp_acceleration_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ramp_acceleration_dialog_set_update;
  applicable->apply = ags_ramp_acceleration_dialog_apply;
  applicable->reset = ags_ramp_acceleration_dialog_reset;
}

void
ags_ramp_acceleration_dialog_init(AgsRampAccelerationDialog *ramp_acceleration_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  ramp_acceleration_dialog->flags = 0;

  g_object_set(ramp_acceleration_dialog,
	       "title", i18n("ramp accelerations"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(ramp_acceleration_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* automation */
  ramp_acceleration_dialog->port = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(ramp_acceleration_dialog->port),
		     FALSE, FALSE,
		     0);  
  
  /* ramp x0 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* ramp x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x0"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* ramp x0 - spin button */
  ramp_acceleration_dialog->ramp_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										       AGS_RAMP_ACCELERATION_MAX_BEATS,
										       0.25);
  gtk_spin_button_set_digits(ramp_acceleration_dialog->ramp_x0,
			     2);
  gtk_spin_button_set_value(ramp_acceleration_dialog->ramp_x0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(ramp_acceleration_dialog->ramp_x0),
		     FALSE, FALSE,
		     0);

  /* ramp y0 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* ramp y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y0"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* ramp y0 - spin button */
  ramp_acceleration_dialog->ramp_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										       1.0,
										       0.001);
  gtk_spin_button_set_value(ramp_acceleration_dialog->ramp_y0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(ramp_acceleration_dialog->ramp_y0),
		     FALSE, FALSE,
		     0);
  
  /* ramp x1 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* ramp x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x1"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* ramp x1 - spin button */
  ramp_acceleration_dialog->ramp_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										       AGS_RAMP_ACCELERATION_MAX_BEATS,
										       0.25);
  gtk_spin_button_set_digits(ramp_acceleration_dialog->ramp_x1,
			     2);
  gtk_spin_button_set_value(ramp_acceleration_dialog->ramp_x1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(ramp_acceleration_dialog->ramp_x1),
		     FALSE, FALSE,
		     0);

  /* ramp y1 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* ramp y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y1"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* ramp y1 - spin button */
  ramp_acceleration_dialog->ramp_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										       1.0,
										       0.001);
  gtk_spin_button_set_value(ramp_acceleration_dialog->ramp_y1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(ramp_acceleration_dialog->ramp_y1),
		     FALSE, FALSE,
		     0);

  /* ramp step count - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* ramp step count - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp step count"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* ramp step count - spin button */
  ramp_acceleration_dialog->ramp_step_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											       AGS_RAMP_ACCELERATION_MAX_STEPS,
											       1.0);
  gtk_spin_button_set_value(ramp_acceleration_dialog->ramp_step_count,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(ramp_acceleration_dialog->ramp_step_count),
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) ramp_acceleration_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_ramp_acceleration_dialog_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) ramp_acceleration_dialog->main_window == main_window){
	return;
      }

      if(ramp_acceleration_dialog->main_window != NULL){
	g_object_unref(ramp_acceleration_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      ramp_acceleration_dialog->main_window = (GtkWidget *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ramp_acceleration_dialog_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, ramp_acceleration_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ramp_acceleration_dialog_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  
  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(connectable);

  if((AGS_RAMP_ACCELERATION_DIALOG_CONNECTED & (ramp_acceleration_dialog->flags)) != 0){
    return;
  }

  ramp_acceleration_dialog->flags |= AGS_RAMP_ACCELERATION_DIALOG_CONNECTED;

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_signal_connect(ramp_acceleration_dialog, "response",
		   G_CALLBACK(ags_ramp_acceleration_dialog_response_callback), ramp_acceleration_dialog);

  g_signal_connect(ramp_acceleration_dialog->port, "changed",
		   G_CALLBACK(ags_ramp_acceleration_dialog_port_callback), ramp_acceleration_dialog);

  /* machine changed */
  if(use_composite_editor){
     AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;
    
    g_signal_connect_after(composite_editor, "machine-changed",
			   G_CALLBACK(ags_ramp_acceleration_dialog_machine_changed_callback), ramp_acceleration_dialog);
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;
    
    g_signal_connect_after(automation_editor, "machine-changed",
			   G_CALLBACK(ags_ramp_acceleration_dialog_machine_changed_callback), ramp_acceleration_dialog);
  }
}

void
ags_ramp_acceleration_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;

  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(connectable);

  if((AGS_RAMP_ACCELERATION_DIALOG_CONNECTED & (ramp_acceleration_dialog->flags)) == 0){
    return;
  }

  ramp_acceleration_dialog->flags &= (~AGS_RAMP_ACCELERATION_DIALOG_CONNECTED);

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_object_disconnect(G_OBJECT(ramp_acceleration_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_ramp_acceleration_dialog_response_callback),
		      ramp_acceleration_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(ramp_acceleration_dialog->port),
		      "any_signal::changed",
		      G_CALLBACK(ags_ramp_acceleration_dialog_port_callback),
		      ramp_acceleration_dialog,
		      NULL);

  /* machine changed */
  if(use_composite_editor){
     AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;
    
    g_object_disconnect(G_OBJECT(composite_editor),
			"any_signal::machine-changed",
			G_CALLBACK(ags_ramp_acceleration_dialog_machine_changed_callback),
			ramp_acceleration_dialog,
			NULL);
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;
    
    g_object_disconnect(G_OBJECT(automation_editor),
			"any_signal::machine-changed",
			G_CALLBACK(ags_ramp_acceleration_dialog_machine_changed_callback),
			ramp_acceleration_dialog,
			NULL);
  }
}

void
ags_ramp_acceleration_dialog_finalize(GObject *gobject)
{
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  ramp_acceleration_dialog = (AgsRampAccelerationDialog *) gobject;
  
  G_OBJECT_CLASS(ags_ramp_acceleration_dialog_parent_class)->finalize(gobject);
}

void
ags_ramp_acceleration_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ramp_acceleration_dialog_apply(AgsApplicable *applicable)
{
  AgsRampAccelerationDialog *ramp_acceleration_dialog;
  AgsWindow *window;
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

  gboolean use_composite_editor;
  gdouble gui_y;
  gdouble tact;
  
  gdouble c_y0, c_y1;
  gdouble val;
  gdouble upper, lower, range, step;
  gdouble c_upper, c_lower, c_range;

  guint tmp;
  guint x0, x1;
  gdouble y0, y1;
  guint step_count;
  gint line;
  guint i, i_start, i_stop;
  guint nth_match, match_count;

  gboolean success;
  
  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;

  notebook = NULL;

  channel_type = G_TYPE_NONE;
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;

    focused_automation_edit = composite_editor->automation_edit->focused_edit;

    if(focused_automation_edit == NULL){
      return;
    }
    
    notebook = composite_editor->automation_edit->channel_selector;

    channel_type = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type;
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;

    machine = automation_editor->selected_machine;

    focused_automation_edit = automation_editor->focused_automation_edit;

    if(focused_automation_edit == NULL){
      return;
    }
    
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

  specifier = gtk_combo_box_text_get_active_text(ramp_acceleration_dialog->port);
  
  audio = machine->audio;

  /* get some values */
  x0 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_acceleration_dialog->ramp_x0);
  y0 = gtk_spin_button_get_value(ramp_acceleration_dialog->ramp_y0);

  x1 = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_acceleration_dialog->ramp_x1);
  y1 = gtk_spin_button_get_value(ramp_acceleration_dialog->ramp_y1);
  
  step_count = gtk_spin_button_get_value_as_int(ramp_acceleration_dialog->ramp_step_count);

  /* remove acceleration of region */
  line = 0;

  g_object_get(audio,
	       "automation", &start_list_automation,
	       NULL);
  
  while(notebook == NULL ||
	(line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
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

      g_object_get(current,
		   "timestamp", &current_timestamp,
		   NULL);

      g_object_unref(current_timestamp);
      
      if(ags_timestamp_get_ags_offset(current_timestamp) < x0){
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
	g_warning("ags_ramp_acceleration_dialog.c - range = 0.0");
	  
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
  line = 0;

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
  
  while(notebook == NULL ||
	(line = ags_notebook_next_active_tab(notebook,
					     line)) != -1){
    AgsChannel *start_channel;
    AgsChannel *nth_channel;
    GList *start_play_port, *start_recall_port;
    GList *play_port, *recall_port;

    guint j;
    guint tmp;
    
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

      if(x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET != 0 &&
	 x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET != (x1 - AGS_AUTOMATION_DEFAULT_OFFSET) / (guint) AGS_AUTOMATION_DEFAULT_OFFSET){
	match_count = 1;

	tmp -= (x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET);
      }

      match_count += (guint) ceil((gdouble) tmp / AGS_AUTOMATION_DEFAULT_OFFSET);
    
      success = FALSE;

      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      for(nth_match = 0; nth_match < match_count; nth_match++){
	list_automation = AGS_PORT(play_port->data)->automation;
	list_automation = ags_automation_find_near_timestamp(list_automation, line,
							     timestamp);

	if(list_automation == NULL){
	  current = ags_automation_new((GObject *) machine->audio,
				       line,
				       channel_type, specifier);
	  g_object_set(current,
		       "port", play_port->data,
		       NULL);

	  current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	  ags_audio_add_automation(machine->audio,
				   (GObject *) current);
	  ags_port_add_automation(AGS_PORT(play_port->data),
				  (GObject *) current);
	}else{
	  current = list_automation->data;
	}

	upper = current->upper;
	lower = current->lower;
	
	range = upper - lower;
	
	if(range == 0.0){
	  g_warning("ags_ramp_acceleration_dialog.c - range = 0.0");
	  
	  break;
	}
	    
	/* ramp value and move offset */
	if(!success){
	  i = 0;

	  if(floor(x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET)){
	    i_stop = step_count;

	    tmp += (x1 - x0);
	  }else{
	    i_stop = tmp / (x1 - x0) * step_count;

	    if(tmp + AGS_AUTOMATION_DEFAULT_OFFSET < x1 - x0){
	      tmp += AGS_AUTOMATION_DEFAULT_OFFSET;
	    }else{
	      tmp = x1 - x0;
	    }
	  }
	
	  success = TRUE;
	}else{
	  i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
	  if(tmp + AGS_AUTOMATION_DEFAULT_OFFSET < x1 - x0){
	    tmp += AGS_AUTOMATION_DEFAULT_OFFSET;
	  }else{
	    tmp = x1 - x0;
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
	
	timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;
      }
    }

    if(recall_port != NULL){
      match_count = 0;
      tmp = x1 - x0;

      if(x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET != 0 &&
	 x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET != (x1 - AGS_AUTOMATION_DEFAULT_OFFSET) / (guint) AGS_AUTOMATION_DEFAULT_OFFSET){
	match_count = 1;

	tmp -= (x0 % (guint) AGS_AUTOMATION_DEFAULT_OFFSET);
      }
      
      match_count += (guint) ceil((gdouble) tmp / AGS_AUTOMATION_DEFAULT_OFFSET);
      
      success = FALSE;

      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      for(nth_match = 0; nth_match < match_count; nth_match++){
	list_automation = AGS_PORT(recall_port->data)->automation;
	list_automation = ags_automation_find_near_timestamp(list_automation, line,
							     timestamp);

	if(list_automation == NULL){
	  current = ags_automation_new((GObject *) machine->audio,
				       line,
				       channel_type, specifier);
	  g_object_set(current,
		       "port", recall_port->data,
		       NULL);

	  current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	  ags_audio_add_automation(machine->audio,
				   (GObject *) current);
	  ags_port_add_automation(AGS_PORT(recall_port->data),
				  (GObject *) current);
	}else{
	  current = list_automation->data;
	}

	upper = current->upper;
	lower = current->lower;
	
	range = upper - lower;
	
	if(range == 0.0){
	  g_warning("ags_ramp_acceleration_dialog.c - range = 0.0");
	  
	  break;
	}
	    
	/* ramp value and move offset */
	if(!success){
	  i = 0;

	  if(floor(x0 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_AUTOMATION_DEFAULT_OFFSET)){
	    i_stop = step_count;

	    tmp += (x1 - x0);
	  }else{
	    i_stop = tmp / (x1 - x0) * step_count;

	    if(tmp + AGS_AUTOMATION_DEFAULT_OFFSET < x1 - x0){
	      tmp += AGS_AUTOMATION_DEFAULT_OFFSET;
	    }else{
	      tmp = x1 - x0;
	    }
	  }
	
	  success = TRUE;
	}else{
	  i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
	  if(tmp + AGS_AUTOMATION_DEFAULT_OFFSET < x1 - x0){
	    tmp += AGS_AUTOMATION_DEFAULT_OFFSET;
	  }else{
	    tmp = x1 - x0;
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

	timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;
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
}

void
ags_ramp_acceleration_dialog_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;

  AgsApplicationContext *application_context;
  
  GList *start_port, *port;

  gchar **collected_specifier;

  gboolean use_composite_editor;
  guint length;

  ramp_acceleration_dialog = AGS_RAMP_ACCELERATION_DIALOG(applicable);

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
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ramp_acceleration_dialog->port))));
  
  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;
  
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
      gtk_combo_box_text_append_text(ramp_acceleration_dialog->port,
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
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(ramp_acceleration_dialog->port,
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
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	gtk_combo_box_text_append_text(ramp_acceleration_dialog->port,
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

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_strfreev(collected_specifier);
}

gboolean
ags_ramp_acceleration_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_ramp_acceleration_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_ramp_acceleration_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new instance of #AgsRampAccelerationDialog.
 *
 * Returns: the new #AgsRampAccelerationDialog
 *
 * Since: 3.0.0
 */
AgsRampAccelerationDialog*
ags_ramp_acceleration_dialog_new(GtkWidget *main_window)
{
  AgsRampAccelerationDialog *ramp_acceleration_dialog;

  ramp_acceleration_dialog = (AgsRampAccelerationDialog *) g_object_new(AGS_TYPE_RAMP_ACCELERATION_DIALOG,
									"main-window", main_window,
									NULL);

  return(ramp_acceleration_dialog);
}
