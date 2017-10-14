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

#include <ags/X/editor/ags_position_notation_cursor_dialog.h>
#include <ags/X/editor/ags_position_notation_cursor_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>

#include <ags/i18n.h>

void ags_position_notation_cursor_dialog_class_init(AgsPositionNotationCursorDialogClass *position_notation_cursor_dialog);
void ags_position_notation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_notation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_notation_cursor_dialog_init(AgsPositionNotationCursorDialog *position_notation_cursor_dialog);
void ags_position_notation_cursor_dialog_set_property(GObject *gobject,
						      guint prop_id,
						      const GValue *value,
						      GParamSpec *param_spec);
void ags_position_notation_cursor_dialog_get_property(GObject *gobject,
						      guint prop_id,
						      GValue *value,
						      GParamSpec *param_spec);
void ags_position_notation_cursor_dialog_finalize(GObject *gobject);
void ags_position_notation_cursor_dialog_connect(AgsConnectable *connectable);
void ags_position_notation_cursor_dialog_disconnect(AgsConnectable *connectable);
void ags_position_notation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_notation_cursor_dialog_apply(AgsApplicable *applicable);
void ags_position_notation_cursor_dialog_reset(AgsApplicable *applicable);
gboolean ags_position_notation_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_position_notation_cursor_dialog
 * @short_description: position tool
 * @title: AgsPositionNotationCursorDialog
 * @section_id:
 * @include: ags/X/editor/ags_position_notation_cursor_dialog.h
 *
 * The #AgsPositionNotationCursorDialog lets you position notation editor cursor.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_position_notation_cursor_dialog_parent_class = NULL;

GType
ags_position_notation_cursor_dialog_get_type(void)
{
  static GType ags_type_position_notation_cursor_dialog = 0;

  if (!ags_type_position_notation_cursor_dialog){
    static const GTypeInfo ags_position_notation_cursor_dialog_info = {
      sizeof (AgsPositionNotationCursorDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_position_notation_cursor_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPositionNotationCursorDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_position_notation_cursor_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_position_notation_cursor_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_position_notation_cursor_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_position_notation_cursor_dialog = g_type_register_static(GTK_TYPE_DIALOG,
								      "AgsPositionNotationCursorDialog", &ags_position_notation_cursor_dialog_info,
								      0);
    
    g_type_add_interface_static(ags_type_position_notation_cursor_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_position_notation_cursor_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return (ags_type_position_notation_cursor_dialog);
}

void
ags_position_notation_cursor_dialog_class_init(AgsPositionNotationCursorDialogClass *position_notation_cursor_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_position_notation_cursor_dialog_parent_class = g_type_class_peek_parent(position_notation_cursor_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) position_notation_cursor_dialog;

  gobject->set_property = ags_position_notation_cursor_dialog_set_property;
  gobject->get_property = ags_position_notation_cursor_dialog_get_property;

  gobject->finalize = ags_position_notation_cursor_dialog_finalize;

  /* properties */
  /**
   * AgsPositionNotationCursorDialog:application-context:
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
   * AgsPositionNotationCursorDialog:main-window:
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
  widget = (GtkWidgetClass *) position_notation_cursor_dialog;

  widget->delete_event = ags_position_notation_cursor_dialog_delete_event;
}

void
ags_position_notation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_position_notation_cursor_dialog_connect;
  connectable->disconnect = ags_position_notation_cursor_dialog_disconnect;
}

void
ags_position_notation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_position_notation_cursor_dialog_set_update;
  applicable->apply = ags_position_notation_cursor_dialog_apply;
  applicable->reset = ags_position_notation_cursor_dialog_reset;
}

void
ags_position_notation_cursor_dialog_init(AgsPositionNotationCursorDialog *position_notation_cursor_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  position_notation_cursor_dialog->flags = 0;

  g_object_set(position_notation_cursor_dialog,
	       "title", i18n("position notation cursor"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) position_notation_cursor_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* set focus */
  position_notation_cursor_dialog->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_toggle_button_set_active(position_notation_cursor_dialog->set_focus,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(position_notation_cursor_dialog->set_focus),
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
  position_notation_cursor_dialog->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												 AGS_POSITION_NOTATION_CURSOR_MAX_BEATS,
												 1.0);
  gtk_spin_button_set_value(position_notation_cursor_dialog->position_x,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(position_notation_cursor_dialog->position_x),
		     FALSE, FALSE,
		     0);
  
  /* position y - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* position y - label */
  label = (GtkLabel *) gtk_label_new(i18n("position y"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* position y - spin button */
  position_notation_cursor_dialog->position_y = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												 AGS_POSITION_NOTATION_CURSOR_MAX_KEYS,
												 1.0);
  gtk_spin_button_set_value(position_notation_cursor_dialog->position_y,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(position_notation_cursor_dialog->position_y),
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) position_notation_cursor_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_position_notation_cursor_dialog_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) position_notation_cursor_dialog->application_context == application_context){
	return;
      }
      
      if(position_notation_cursor_dialog->application_context != NULL){
	g_object_unref(position_notation_cursor_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      position_notation_cursor_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) position_notation_cursor_dialog->main_window == main_window){
	return;
      }

      if(position_notation_cursor_dialog->main_window != NULL){
	g_object_unref(position_notation_cursor_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      position_notation_cursor_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_notation_cursor_dialog_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, position_notation_cursor_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, position_notation_cursor_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_notation_cursor_dialog_connect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_NOTATION_CURSOR_DIALOG_CONNECTED & (position_notation_cursor_dialog->flags)) != 0){
    return;
  }

  position_notation_cursor_dialog->flags |= AGS_POSITION_NOTATION_CURSOR_DIALOG_CONNECTED;

  g_signal_connect(position_notation_cursor_dialog, "response",
		   G_CALLBACK(ags_position_notation_cursor_dialog_response_callback), position_notation_cursor_dialog);
}

void
ags_position_notation_cursor_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_NOTATION_CURSOR_DIALOG_CONNECTED & (position_notation_cursor_dialog->flags)) == 0){
    return;
  }

  position_notation_cursor_dialog->flags &= (~AGS_POSITION_NOTATION_CURSOR_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(position_notation_cursor_dialog),
		      "response",
		      G_CALLBACK(ags_position_notation_cursor_dialog_response_callback),
		      position_notation_cursor_dialog,
		      NULL);
}

void
ags_position_notation_cursor_dialog_finalize(GObject *gobject)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = (AgsPositionNotationCursorDialog *) gobject;

  if(position_notation_cursor_dialog->application_context != NULL){
    g_object_unref(position_notation_cursor_dialog->application_context);
  }
  
  G_OBJECT_CLASS(ags_position_notation_cursor_dialog_parent_class)->finalize(gobject);
}

void
ags_position_notation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_position_notation_cursor_dialog_apply(AgsApplicable *applicable)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  AgsWindow *window;
  AgsEditor *editor;
  AgsToolbar *toolbar;
  AgsMachine *machine;
  GtkWidget *widget;
  
  GtkAdjustment *vadjustment, *hadjustment;

  gdouble zoom;
  guint map_height, height;
  guint history;
  guint x, y;
  
  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(applicable);

  window = position_notation_cursor_dialog->main_window;
  editor = window->editor;

  machine = editor->selected_machine;

  if(machine == NULL ||
     editor->current_edit_widget == NULL){
    return;
  }

  toolbar = editor->toolbar;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(toolbar->zoom));
  zoom = exp2((double) history - 2.0);

  if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
    AgsPatternEdit *pattern_edit;

    pattern_edit = editor->current_edit_widget;

    x = gtk_spin_button_get_value_as_int(position_notation_cursor_dialog->position_x);
    pattern_edit->selected_x = 16 * x;
    y = 
      pattern_edit->selected_y = gtk_spin_button_get_value_as_int(position_notation_cursor_dialog->position_y);

    vadjustment = GTK_RANGE(pattern_edit->vscrollbar)->adjustment;
    hadjustment = GTK_RANGE(pattern_edit->hscrollbar)->adjustment;

    widget = pattern_edit->drawing_area;
    
    map_height = pattern_edit->map_height;
    height = widget->allocation.height;
  }else if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
    AgsNoteEdit *note_edit;

    note_edit = editor->current_edit_widget;
    
    x = gtk_spin_button_get_value_as_int(position_notation_cursor_dialog->position_x);
    note_edit->selected_x = 16 * x;
    y = 
      note_edit->selected_y = gtk_spin_button_get_value_as_int(position_notation_cursor_dialog->position_y);

    vadjustment = GTK_RANGE(note_edit->vscrollbar)->adjustment;
    hadjustment = GTK_RANGE(note_edit->hscrollbar)->adjustment;

    widget = note_edit->drawing_area;
    
    map_height = note_edit->map_height;
    height = widget->allocation.height;
  }

  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     (x * 16 * 64 / zoom) * (hadjustment->upper / (AGS_EDITOR_MAX_CONTROLS * 16 * 16 * 64 / zoom)));
  }

  if(vadjustment != NULL){
    gtk_adjustment_set_value(vadjustment,
			     (y * 14) * (vadjustment->upper / map_height));
  }

  if(gtk_toggle_button_get_active(position_notation_cursor_dialog->set_focus)){
    gtk_widget_grab_focus(widget);
  }
}

void
ags_position_notation_cursor_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_position_notation_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_position_notation_cursor_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_position_notation_cursor_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsPositionNotationCursorDialog.
 *
 * Returns: a new #AgsPositionNotationCursorDialog
 *
 * Since: 1.1.0
 */
AgsPositionNotationCursorDialog*
ags_position_notation_cursor_dialog_new(GtkWidget *main_window)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = (AgsPositionNotationCursorDialog *) g_object_new(AGS_TYPE_POSITION_NOTATION_CURSOR_DIALOG,
										     "main-window", main_window,
										     NULL);

  return(position_notation_cursor_dialog);
}
