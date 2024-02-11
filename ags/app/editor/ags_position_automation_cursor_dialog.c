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

#include <ags/app/editor/ags_position_automation_cursor_dialog.h>
#include <ags/app/editor/ags_position_automation_cursor_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <math.h>

#include <ags/i18n.h>

void ags_position_automation_cursor_dialog_class_init(AgsPositionAutomationCursorDialogClass *position_automation_cursor_dialog);
void ags_position_automation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_automation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_automation_cursor_dialog_init(AgsPositionAutomationCursorDialog *position_automation_cursor_dialog);
void ags_position_automation_cursor_dialog_finalize(GObject *gobject);

gboolean ags_position_automation_cursor_dialog_is_connected(AgsConnectable *connectable);
void ags_position_automation_cursor_dialog_connect(AgsConnectable *connectable);
void ags_position_automation_cursor_dialog_disconnect(AgsConnectable *connectable);

void ags_position_automation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_automation_cursor_dialog_apply(AgsApplicable *applicable);
void ags_position_automation_cursor_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_position_automation_cursor_dialog
 * @short_description: position tool
 * @title: AgsPositionAutomationCursorDialog
 * @section_id:
 * @include: ags/app/editor/ags_position_automation_cursor_dialog.h
 *
 * The #AgsPositionAutomationCursorDialog lets you position automation editor cursor.
 */

static gpointer ags_position_automation_cursor_dialog_parent_class = NULL;

GType
ags_position_automation_cursor_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_position_automation_cursor_dialog = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_position_automation_cursor_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_position_automation_cursor_dialog_class_init(AgsPositionAutomationCursorDialogClass *position_automation_cursor_dialog)
{
  GObjectClass *gobject;

  ags_position_automation_cursor_dialog_parent_class = g_type_class_peek_parent(position_automation_cursor_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) position_automation_cursor_dialog;

  gobject->finalize = ags_position_automation_cursor_dialog_finalize;
}

void
ags_position_automation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_position_automation_cursor_dialog_is_connected;  
  connectable->connect = ags_position_automation_cursor_dialog_connect;
  connectable->disconnect = ags_position_automation_cursor_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  position_automation_cursor_dialog->connectable_flags = 0;

  gtk_window_set_hide_on_close((GtkWindow *) position_automation_cursor_dialog,
			       TRUE);
  
  g_object_set(position_automation_cursor_dialog,
	       "title", i18n("position automation cursor"),
	       NULL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) position_automation_cursor_dialog),
		 GTK_WIDGET(vbox));  

  /* set focus */
  position_automation_cursor_dialog->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_check_button_set_active((GtkCheckButton *) position_automation_cursor_dialog->set_focus,
			      TRUE);
  gtk_box_append(vbox,
		 (GtkWidget *) position_automation_cursor_dialog->set_focus);

  /* position x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* position x - label */
  label = (GtkLabel *) gtk_label_new(i18n("position x"));
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) label);

  /* position x - spin button */
  position_automation_cursor_dialog->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												   AGS_POSITION_AUTOMATION_CURSOR_MAX_BEATS,
												   1.0);
  gtk_spin_button_set_value(position_automation_cursor_dialog->position_x,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) position_automation_cursor_dialog->position_x);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) position_automation_cursor_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_position_automation_cursor_dialog_finalize(GObject *gobject)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = (AgsPositionAutomationCursorDialog *) gobject;
  
  G_OBJECT_CLASS(ags_position_automation_cursor_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_position_automation_cursor_dialog_is_connected(AgsConnectable *connectable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;
  
  gboolean is_connected;
  
  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (position_automation_cursor_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_position_automation_cursor_dialog_connect(AgsConnectable *connectable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  position_automation_cursor_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect((GObject *) position_automation_cursor_dialog, "response",
		   G_CALLBACK(ags_position_automation_cursor_dialog_response_callback), (gpointer) position_automation_cursor_dialog);
}

void
ags_position_automation_cursor_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  position_automation_cursor_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(position_automation_cursor_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_position_automation_cursor_dialog_response_callback),
		      position_automation_cursor_dialog,
		      NULL);
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
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsAutomationEdit *focused_automation_edit;
  GtkWidget *widget;

  GtkAdjustment *hadjustment;
  
  AgsApplicationContext *application_context;

  gdouble zoom;
  guint history;
  guint x;

  position_automation_cursor_dialog = AGS_POSITION_AUTOMATION_CURSOR_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = window->composite_editor;

  composite_toolbar = composite_editor->toolbar;

  focused_automation_edit = (AgsAutomationEdit *) composite_editor->automation_edit->focused_edit;

  machine = composite_editor->selected_machine;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(composite_toolbar->zoom));
  
  if(machine == NULL){
    return;
  }
  
  zoom = exp2((double) history - 2.0);

  if(focused_automation_edit == NULL){
    return;
  }
  
  x = gtk_spin_button_get_value_as_int(position_automation_cursor_dialog->position_x);
  focused_automation_edit->cursor_position_x = 16 * x;
  focused_automation_edit->cursor_position_y = 0.0;

  hadjustment = gtk_range_get_adjustment(GTK_RANGE(focused_automation_edit->hscrollbar));

  widget = (GtkWidget *) focused_automation_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (gtk_adjustment_get_upper(hadjustment) / (AGS_AUTOMATION_DEFAULT_LENGTH / zoom))));
  }
  
  if(gtk_check_button_get_active(position_automation_cursor_dialog->set_focus)){
    gtk_widget_grab_focus(widget);
  }
}

void
ags_position_automation_cursor_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_position_automation_cursor_dialog_new:
 * @transient_for: the transient #AgsWindow
 *
 * Create a new #AgsPositionAutomationCursorDialog.
 *
 * Returns: a new #AgsPositionAutomationCursorDialog
 *
 * Since: 3.0.0
 */
AgsPositionAutomationCursorDialog*
ags_position_automation_cursor_dialog_new(GtkWindow *transient_for)
{
  AgsPositionAutomationCursorDialog *position_automation_cursor_dialog;

  position_automation_cursor_dialog = (AgsPositionAutomationCursorDialog *) g_object_new(AGS_TYPE_POSITION_AUTOMATION_CURSOR_DIALOG,
											 "transient-for", transient_for,
											 NULL);

  return(position_automation_cursor_dialog);
}
