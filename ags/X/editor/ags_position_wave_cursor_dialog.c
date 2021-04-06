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

#include <ags/X/editor/ags_position_wave_cursor_dialog.h>
#include <ags/X/editor/ags_position_wave_cursor_dialog_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_wave_toolbar.h>
#include <ags/X/editor/ags_wave_edit.h>

#include <math.h>

#include <ags/i18n.h>

void ags_position_wave_cursor_dialog_class_init(AgsPositionWaveCursorDialogClass *position_wave_cursor_dialog);
void ags_position_wave_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_wave_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_wave_cursor_dialog_init(AgsPositionWaveCursorDialog *position_wave_cursor_dialog);
void ags_position_wave_cursor_dialog_set_property(GObject *gobject,
						  guint prop_id,
						  const GValue *value,
						  GParamSpec *param_spec);
void ags_position_wave_cursor_dialog_get_property(GObject *gobject,
						  guint prop_id,
						  GValue *value,
						  GParamSpec *param_spec);
void ags_position_wave_cursor_dialog_finalize(GObject *gobject);
void ags_position_wave_cursor_dialog_connect(AgsConnectable *connectable);
void ags_position_wave_cursor_dialog_disconnect(AgsConnectable *connectable);
void ags_position_wave_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_wave_cursor_dialog_apply(AgsApplicable *applicable);
void ags_position_wave_cursor_dialog_reset(AgsApplicable *applicable);
gboolean ags_position_wave_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_position_wave_cursor_dialog
 * @short_description: position tool
 * @title: AgsPositionWaveCursorDialog
 * @section_id:
 * @include: ags/X/editor/ags_position_wave_cursor_dialog.h
 *
 * The #AgsPositionWaveCursorDialog lets you position wave editor cursor.
 */

enum{
  PROP_0,
  PROP_MAIN_WINDOW,
};

static gpointer ags_position_wave_cursor_dialog_parent_class = NULL;

GType
ags_position_wave_cursor_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_position_wave_cursor_dialog = 0;

    static const GTypeInfo ags_position_wave_cursor_dialog_info = {
      sizeof (AgsPositionWaveCursorDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_position_wave_cursor_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPositionWaveCursorDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_position_wave_cursor_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_position_wave_cursor_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_position_wave_cursor_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_position_wave_cursor_dialog = g_type_register_static(GTK_TYPE_DIALOG,
								  "AgsPositionWaveCursorDialog", &ags_position_wave_cursor_dialog_info,
								  0);
    
    g_type_add_interface_static(ags_type_position_wave_cursor_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_position_wave_cursor_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_position_wave_cursor_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_position_wave_cursor_dialog_class_init(AgsPositionWaveCursorDialogClass *position_wave_cursor_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_position_wave_cursor_dialog_parent_class = g_type_class_peek_parent(position_wave_cursor_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) position_wave_cursor_dialog;

  gobject->set_property = ags_position_wave_cursor_dialog_set_property;
  gobject->get_property = ags_position_wave_cursor_dialog_get_property;

  gobject->finalize = ags_position_wave_cursor_dialog_finalize;

  /* properties */
  /**
   * AgsPositionWaveCursorDialog:main-window:
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
  widget = (GtkWidgetClass *) position_wave_cursor_dialog;

  widget->delete_event = ags_position_wave_cursor_dialog_delete_event;
}

void
ags_position_wave_cursor_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_position_wave_cursor_dialog_connect;
  connectable->disconnect = ags_position_wave_cursor_dialog_disconnect;
}

void
ags_position_wave_cursor_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_position_wave_cursor_dialog_set_update;
  applicable->apply = ags_position_wave_cursor_dialog_apply;
  applicable->reset = ags_position_wave_cursor_dialog_reset;
}

void
ags_position_wave_cursor_dialog_init(AgsPositionWaveCursorDialog *position_wave_cursor_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  position_wave_cursor_dialog->flags = 0;

  g_object_set(position_wave_cursor_dialog,
	       "title", i18n("position wave cursor"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(position_wave_cursor_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* set focus */
  position_wave_cursor_dialog->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_toggle_button_set_active((GtkToggleButton *) position_wave_cursor_dialog->set_focus,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(position_wave_cursor_dialog->set_focus),
		     FALSE, FALSE,
		     0);  

  /* position x - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
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
  position_wave_cursor_dialog->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											     AGS_POSITION_WAVE_CURSOR_MAX_BEATS,
											     1.0);
  gtk_spin_button_set_value(position_wave_cursor_dialog->position_x,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(position_wave_cursor_dialog->position_x),
		     FALSE, FALSE,
		     0);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) position_wave_cursor_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_position_wave_cursor_dialog_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = AGS_POSITION_WAVE_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) position_wave_cursor_dialog->main_window == main_window){
	return;
      }

      if(position_wave_cursor_dialog->main_window != NULL){
	g_object_unref(position_wave_cursor_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      position_wave_cursor_dialog->main_window = (GtkWidget *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_wave_cursor_dialog_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = AGS_POSITION_WAVE_CURSOR_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, position_wave_cursor_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_position_wave_cursor_dialog_connect(AgsConnectable *connectable)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = AGS_POSITION_WAVE_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_WAVE_CURSOR_DIALOG_CONNECTED & (position_wave_cursor_dialog->flags)) != 0){
    return;
  }

  position_wave_cursor_dialog->flags |= AGS_POSITION_WAVE_CURSOR_DIALOG_CONNECTED;

  g_signal_connect(position_wave_cursor_dialog, "response",
		   G_CALLBACK(ags_position_wave_cursor_dialog_response_callback), position_wave_cursor_dialog);
}

void
ags_position_wave_cursor_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = AGS_POSITION_WAVE_CURSOR_DIALOG(connectable);

  if((AGS_POSITION_WAVE_CURSOR_DIALOG_CONNECTED & (position_wave_cursor_dialog->flags)) == 0){
    return;
  }

  position_wave_cursor_dialog->flags &= (~AGS_POSITION_WAVE_CURSOR_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(position_wave_cursor_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_position_wave_cursor_dialog_response_callback),
		      position_wave_cursor_dialog,
		      NULL);
}

void
ags_position_wave_cursor_dialog_finalize(GObject *gobject)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = (AgsPositionWaveCursorDialog *) gobject;
  
  G_OBJECT_CLASS(ags_position_wave_cursor_dialog_parent_class)->finalize(gobject);
}

void
ags_position_wave_cursor_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_position_wave_cursor_dialog_apply(AgsApplicable *applicable)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  AgsWindow *window;
  AgsWaveWindow *wave_window;
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsWaveEdit *wave_edit;
  AgsMachine *machine;
  GtkWidget *widget;

  GtkAdjustment *hadjustment;

  gdouble zoom;
  guint history;
  guint x;

  position_wave_cursor_dialog = AGS_POSITION_WAVE_CURSOR_DIALOG(applicable);

  window = (AgsWindow *) position_wave_cursor_dialog->main_window;

  wave_window = window->wave_window;
  wave_editor = wave_window->wave_editor;

  machine = wave_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  wave_toolbar = wave_editor->wave_toolbar;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(wave_toolbar->zoom));
  zoom = exp2((double) history - 2.0);
  
  wave_edit = wave_editor->focused_wave_edit;

  if(wave_edit == NULL){
    return;
  }
  
  x = gtk_spin_button_get_value_as_int(position_wave_cursor_dialog->position_x);
  wave_edit->cursor_position_x = 16 * x;
  wave_edit->cursor_position_y = 0.0;

  hadjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->hscrollbar));

  widget = (GtkWidget *) wave_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (gtk_adjustment_get_upper(hadjustment) / (AGS_WAVE_EDITOR_MAX_CONTROLS / zoom))));
  }
  
  if(gtk_toggle_button_get_active((GtkToggleButton *) position_wave_cursor_dialog->set_focus)){
    gtk_widget_grab_focus(widget);
  }
}

void
ags_position_wave_cursor_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_position_wave_cursor_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_position_wave_cursor_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_position_wave_cursor_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsPositionWaveCursorDialog.
 *
 * Returns: a new #AgsPositionWaveCursorDialog
 *
 * Since: 3.0.0
 */
AgsPositionWaveCursorDialog*
ags_position_wave_cursor_dialog_new(GtkWidget *main_window)
{
  AgsPositionWaveCursorDialog *position_wave_cursor_dialog;

  position_wave_cursor_dialog = (AgsPositionWaveCursorDialog *) g_object_new(AGS_TYPE_POSITION_WAVE_CURSOR_DIALOG,
									     "main-window", main_window,
									     NULL);

  return(position_wave_cursor_dialog);
}
