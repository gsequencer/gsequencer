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

#include <ags/app/editor/ags_position_notation_cursor_dialog.h>
#include <ags/app/editor/ags_position_notation_cursor_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <ags/i18n.h>

void ags_position_notation_cursor_dialog_class_init(AgsPositionNotationCursorDialogClass *position_notation_cursor_dialog);
void ags_position_notation_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_notation_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_notation_cursor_dialog_init(AgsPositionNotationCursorDialog *position_notation_cursor_dialog);
void ags_position_notation_cursor_dialog_finalize(GObject *gobject);

void ags_position_notation_cursor_dialog_connect(AgsConnectable *connectable);
void ags_position_notation_cursor_dialog_disconnect(AgsConnectable *connectable);

void ags_position_notation_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_notation_cursor_dialog_apply(AgsApplicable *applicable);
void ags_position_notation_cursor_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_position_notation_cursor_dialog
 * @short_description: position tool
 * @title: AgsPositionNotationCursorDialog
 * @section_id:
 * @include: ags/app/editor/ags_position_notation_cursor_dialog.h
 *
 * The #AgsPositionNotationCursorDialog lets you position notation editor cursor.
 */

static gpointer ags_position_notation_cursor_dialog_parent_class = NULL;

GType
ags_position_notation_cursor_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_position_notation_cursor_dialog = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_position_notation_cursor_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_position_notation_cursor_dialog_class_init(AgsPositionNotationCursorDialogClass *position_notation_cursor_dialog)
{
  GObjectClass *gobject;

  ags_position_notation_cursor_dialog_parent_class = g_type_class_peek_parent(position_notation_cursor_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) position_notation_cursor_dialog;

  gobject->finalize = ags_position_notation_cursor_dialog_finalize;
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
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  position_notation_cursor_dialog->connectable_flags = 0;

  gtk_window_set_hide_on_close(position_notation_cursor_dialog,
			       TRUE);
  
  g_object_set(position_notation_cursor_dialog,
	       "title", i18n("position notation cursor"),
	       NULL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area(position_notation_cursor_dialog),
		 GTK_WIDGET(vbox));  

  /* set focus */
  position_notation_cursor_dialog->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_check_button_set_active(position_notation_cursor_dialog->set_focus,
			      TRUE);
  gtk_box_append(vbox,
		 GTK_WIDGET(position_notation_cursor_dialog->set_focus));  

  /* position x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* position x - label */
  label = (GtkLabel *) gtk_label_new(i18n("position x"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* position x - spin button */
  position_notation_cursor_dialog->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												 AGS_POSITION_NOTATION_CURSOR_MAX_BEATS,
												 1.0);
  gtk_spin_button_set_value(position_notation_cursor_dialog->position_x,
			    0.0);
  gtk_box_append((GtkBox *) hbox,
		 GTK_WIDGET(position_notation_cursor_dialog->position_x));
  
  /* position y - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* position y - label */
  label = (GtkLabel *) gtk_label_new(i18n("position y"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* position y - spin button */
  position_notation_cursor_dialog->position_y = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												 AGS_POSITION_NOTATION_CURSOR_MAX_KEYS,
												 1.0);
  gtk_spin_button_set_value(position_notation_cursor_dialog->position_y,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(position_notation_cursor_dialog->position_y));

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) position_notation_cursor_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_position_notation_cursor_dialog_connect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (position_notation_cursor_dialog->connectable_flags)) != 0){
    return;
  }

  position_notation_cursor_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(position_notation_cursor_dialog, "response",
		   G_CALLBACK(ags_position_notation_cursor_dialog_response_callback), position_notation_cursor_dialog);
}

void
ags_position_notation_cursor_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (position_notation_cursor_dialog->connectable_flags)) == 0){
    return;
  }

  position_notation_cursor_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(position_notation_cursor_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_position_notation_cursor_dialog_response_callback),
		      position_notation_cursor_dialog,
		      NULL);
}

void
ags_position_notation_cursor_dialog_finalize(GObject *gobject)
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = (AgsPositionNotationCursorDialog *) gobject;
  
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
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  GtkWidget *editor;
  AgsNotationEdit *notation_edit;
  GtkWidget *widget;
  
  GtkAdjustment *hadjustment;
  
  AgsApplicationContext *application_context;

  gdouble zoom;
  guint map_height, height;
  guint history;
  guint x, y;
  
  position_notation_cursor_dialog = AGS_POSITION_NOTATION_CURSOR_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  composite_toolbar = composite_editor->toolbar;
    
  machine = composite_editor->selected_machine;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(composite_toolbar->zoom));
  
  if(machine == NULL){
    return;
  }
  
  zoom = exp2((double) history - 2.0);
  
  x = gtk_spin_button_get_value_as_int(position_notation_cursor_dialog->position_x);

  notation_edit = AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit;

  if(notation_edit != NULL){
    notation_edit->cursor_position_x = 16 * x;
    notation_edit->cursor_position_y = 0.0;
  }

  hadjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));

  widget = (GtkWidget *) notation_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (gtk_adjustment_get_upper(hadjustment) / (AGS_NOTATION_DEFAULT_LENGTH / zoom))));
  }

  if(gtk_toggle_button_get_active((GtkToggleButton *) position_notation_cursor_dialog->set_focus)){
    gtk_widget_grab_focus(widget);
  }
  //TODO:JK: implement me
}

void
ags_position_notation_cursor_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_position_notation_cursor_dialog_new:
 *
 * Create a new #AgsPositionNotationCursorDialog.
 *
 * Returns: a new #AgsPositionNotationCursorDialog
 *
 * Since: 3.0.0
 */
AgsPositionNotationCursorDialog*
ags_position_notation_cursor_dialog_new()
{
  AgsPositionNotationCursorDialog *position_notation_cursor_dialog;

  position_notation_cursor_dialog = (AgsPositionNotationCursorDialog *) g_object_new(AGS_TYPE_POSITION_NOTATION_CURSOR_DIALOG,
										     NULL);

  return(position_notation_cursor_dialog);
}
