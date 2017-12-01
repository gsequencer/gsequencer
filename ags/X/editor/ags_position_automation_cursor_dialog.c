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

#include <ags/X/editor/ags_position_automation_cursor_dialog.h>
#include <ags/X/editor/ags_position_automation_cursor_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_automation_edit.h>

#include <math.h>

#include <ags/i18n.h>

void ags_position_automation_cursor_dialog_class_init(AgsPositionAutomationCursorDialogClass *position_automation_cursor_dialog);
void ags_position_automation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_automation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_automation_cursor_dialog_init(AgsPositionAutomationCursorDialog *position_automation_cursor_dialog);
void ags_position_automation_cursor_dialog_set_property(GObject *gobject,
							guint prop_id,
							const GValue *value,
							GParamSpec *param_spec);
void ags_position_automation_cursor_dialog_get_property(GObject *gobject,
							guint prop_id,
							GValue *value,
							GParamSpec *param_spec);
void ags_position_automation_cursor_dialog_finalize(GObject *gobject);
void ags_position_automation_cursor_dialog_connect(AgsConnectable *connectable);
void ags_position_automation_cursor_dialog_disconnect(AgsConnectable *connectable);
void ags_position_automation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_automation_cursor_dialog_apply(AgsApplicable *applicable);
void ags_position_automation_cursor_dialog_reset(AgsApplicable *applicable);
gboolean ags_position_automation_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_position_automation_cursor_dialog
 * @short_description: position tool
 * @title: AgsPositionAutomationCursorDialog
 * @section_id:
 * @include: ags/X/editor/ags_position_automation_cursor_dialog.h
 *
 * The #AgsPositionAutomationCursorDialog lets you position automation editor cursor.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_position_automation_cursor_dialog_parent_class = NULL;

GType
ags_position_automation_cursor_dialog_get_type(void)
{
  static GType ags_type_position_automation_cursor_dialog = 0;

  if (!ags_type_position_automation_cursor_dialog){
    static const GTypeInfo ags_position_automation_cursor_dialog_info = {
      sizeof (AgsPositionAutomationCursorDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_position_automation_cursor_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPositionAutomationCursorDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_position_automation_cursor_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_position_automation_cursor_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_position_automation_cursor_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_position_automation_cursor_dialog = g_type_register_static(GTK_TYPE_DIALOG,
									"AgsPositionAutomationCursorDialog", &ags_position_automation_cursor_dialog_info,
									0);
    
    g_type_add_interface_static(ags_type_position_automation_cursor_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_position_automation_cursor_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return (ags_type_position_automation_cursor_dialog);
}

void
ags_position_automation_cursor_dialog_class_init(AgsPositionAutomationCursorDialogClass *position_automation_cursor_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_position_automation_cursor_dialog_parent_class = g_type_class_peek_parent(position_automation_cursor_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) position_automation_cursor_dialog;

  gobject->set_property = ags_position_automation_cursor_dialog_set_property;
  gobject->get_property = ags_position_automation_cursor_dialog_get_property;

  gobject->finalize = ags_position_automation_cursor_dialog_finalize;

  /* properties */
  /**
   * AgsPositionAutomationCursorDialog:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 1.1.0
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
   * AgsPositionAutomationCursorDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 1.1.0
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
  widget = (GtkWidgetClass *) position_automation_cursor_dialog;

  widget->delete_event = ags_position_automation_cursor_dialog_delete_event;
}

void
ags_position_automation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_position_automation_cursor_dialog_connect;
  connectable->disconnect = ags_position_automation_cursor_dialog_disconnect;
}

void
ags_position_automation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_position_automation_cursor_dialog_set_update;
  applicable->apply = ags_position_automation_cursor_dialog_apply;
  applicable->reset = ags_position_automation_cursor_dialog_reset;
}

void
ags_position_automation_cursor_dialog_init(AgsPositionAutomationCursorDialog *position_automation_cursor_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  position_automation_cursor_dialog->flags = 0;

  g_object_set(position_automation_cursor_dialog,
	       "title", i18n("position automation cursor"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) position_automation_cursor_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* set focus */
  position_automation_cursor_dialog->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_toggle_button_set_active(position_automation_cursor_dialog->set_focus,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(position_automation_cursor_dialog->set_focus),
		     FALSE, FALSE,
		     0);  

  /* position x - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* position x - label */
  label = (GtkLabel *) gtk_label_new(i18n("position x"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* position x - spin button */
  position_automation_cursor_dialog->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												   AGS_POSITION_AUTOMATION_CURSOR_MAX_BEATS,
												   1.0);
  gtk_spin_button_set_value(position_automation_cursor_dialog->position_x,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(position_automation_cursor_dialog->position_x),
		     FALSE, FALSE,
		     0);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) position_automation_cursor_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_position_automation_cursor_dialog_set_property(GObject *gobject,
						   guint prop_id,
						   const GValue *value,
						   GParamSpec *param_spec)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) position_automation_cursor_dialog->application_context == application_context){
	return;
      }
      
      if(position_automation_cursor_dialog->application_context != NULL){
	g_object_unref(position_automation_cursor_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      position_automation_cursor_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) position_automation_cursor_dialog->main_window == main_window){
	return;
      }

      if(position_automation_cursor_dialog->main_window != NULL){
	g_object_unref(position_automation_cursor_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      position_automation_cursor_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_automation_cursor_dialog_get_property(GObject *gobject,
						   guint prop_id,
						   GValue *value,
						   GParamSpec *param_spec)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, position_automation_cursor_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, position_automation_cursor_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_automation_cursor_dialog_connect(AgsConnectable *connectable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CONNECTED & (position_automation_cursor_dialog->flags)) != 0){
    return;
  }

  position_automation_cursor_dialog->flags |= AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CONNECTED;

  g_signal_connect(position_automation_cursor_dialog, "response",
		   G_CALLBACK(ags_position_automation_cursor_dialog_response_callback), position_automation_cursor_dialog);
}

void
ags_position_automation_cursor_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CONNECTED & (position_automation_cursor_dialog->flags)) == 0){
    return;
  }

  position_automation_cursor_dialog->flags &= (~AGS_POSITION_AUTOMATION_CURSOR_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(position_automation_cursor_dialog),
		      "response",
		      G_CALLBACK(ags_position_automation_cursor_dialog_response_callback),
		      position_automation_cursor_dialog,
		      NULL);
}

void
ags_position_automation_cursor_dialog_finalize(GObject *gobject)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = (AgsPositionAutomationCursorDialog *) gobject;

  if(position_automation_cursor_dialog->application_context != NULL){
    g_object_unref(position_automation_cursor_dialog->application_context);
  }
  
  G_OBJECT_CLASS(ags_position_automation_cursor_dialog_parent_class)->finalize(gobject);
}

void
ags_position_automation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_position_automation_cursor_dialog_apply(AgsApplicable *applicable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  AgsWindow *window;
  AgsAutomationWindow *automation_window;
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsAutomationEdit *automation_edit;
  AgsMachine *machine;
  GtkWidget *widget;

  GtkAdjustment *hadjustment;

  gdouble zoom;
  guint history;
  gint current_page;
  guint x;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(applicable);

  window = position_automation_cursor_dialog->main_window;

  automation_window = window->automation_window;
  automation_editor = automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  automation_toolbar = automation_editor->automation_toolbar;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(automation_toolbar->zoom));
  zoom = exp2((double) history - 2.0);

  current_page = gtk_notebook_get_current_page(automation_editor->notebook);
  
  if(current_page == 0){
    automation_edit = automation_editor->current_audio_automation_edit;
  }else if(current_page == 1){
    automation_edit = automation_editor->current_output_automation_edit;
  }else{
    automation_edit = automation_editor->current_input_automation_edit;
  }

  if(automation_edit == NULL){
    return;
  }
  
  x = gtk_spin_button_get_value_as_int(position_automation_cursor_dialog->position_x);
  automation_edit->edit_x = 16 * x;
  automation_edit->edit_y = 0;

  hadjustment = GTK_RANGE(automation_edit->hscrollbar)->adjustment;

  widget = automation_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (hadjustment->upper / (AGS_NOTATION_EDITOR_MAX_CONTROLS * 64 / zoom))));
  }
  
  if(gtk_toggle_button_get_active(position_automation_cursor_dialog->set_focus)){
    gtk_widget_grab_focus(widget);
  }
}

void
ags_position_automation_cursor_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_position_automation_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_position_automation_cursor_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_position_automation_cursor_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsPositionAutomationCursorDialog.
 *
 * Returns: a new #AgsPositionAutomationCursorDialog
 *
 * Since: 1.1.0
 */
AgsPositionAutomationCursorDialog*
ags_position_automation_cursor_dialog_new(GtkWidget *main_window)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = (AgsPositionAutomationCursorDialog *) g_object_new(AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG,
											 "main-window", main_window,
											 NULL);

  return(position_automation_cursor_dialog);
}
