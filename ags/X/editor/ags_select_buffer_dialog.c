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

#include <ags/X/editor/ags_select_buffer_dialog.h>
#include <ags/X/editor/ags_select_buffer_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/i18n.h>

void ags_select_buffer_dialog_class_init(AgsSelectBufferDialogClass *select_buffer_dialog);
void ags_select_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_buffer_dialog_init(AgsSelectBufferDialog *select_buffer_dialog);
void ags_select_buffer_dialog_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_select_buffer_dialog_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_select_buffer_dialog_finalize(GObject *gobject);

void ags_select_buffer_dialog_connect(AgsConnectable *connectable);
void ags_select_buffer_dialog_disconnect(AgsConnectable *connectable);

void ags_select_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_buffer_dialog_apply(AgsApplicable *applicable);
void ags_select_buffer_dialog_reset(AgsApplicable *applicable);
gboolean ags_select_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_select_buffer_dialog
 * @short_description: select tool
 * @title: AgsSelectBufferDialog
 * @section_id:
 * @include: ags/X/editor/ags_select_buffer_dialog.h
 *
 * The #AgsSelectBufferDialog lets you select buffers.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_select_buffer_dialog_parent_class = NULL;

GType
ags_select_buffer_dialog_get_type(void)
{
  static GType ags_type_select_buffer_dialog = 0;

  if (!ags_type_select_buffer_dialog){
    static const GTypeInfo ags_select_buffer_dialog_info = {
      sizeof (AgsSelectBufferDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_buffer_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectBufferDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_buffer_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_buffer_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_buffer_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_buffer_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							   "AgsSelectBufferDialog", &ags_select_buffer_dialog_info,
							   0);
    
    g_type_add_interface_static(ags_type_select_buffer_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_buffer_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return (ags_type_select_buffer_dialog);
}

void
ags_select_buffer_dialog_class_init(AgsSelectBufferDialogClass *select_buffer_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_select_buffer_dialog_parent_class = g_type_class_peek_parent(select_buffer_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) select_buffer_dialog;

  gobject->set_property = ags_select_buffer_dialog_set_property;
  gobject->get_property = ags_select_buffer_dialog_get_property;

  gobject->finalize = ags_select_buffer_dialog_finalize;

  /* properties */
  /**
   * AgsSelectBufferDialog:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 2.0.0
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
   * AgsSelectBufferDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 2.0.0
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
  widget = (GtkWidgetClass *) select_buffer_dialog;

  widget->delete_event = ags_select_buffer_dialog_delete_event;
}

void
ags_select_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_select_buffer_dialog_connect;
  connectable->disconnect = ags_select_buffer_dialog_disconnect;
}

void
ags_select_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_buffer_dialog_set_update;
  applicable->apply = ags_select_buffer_dialog_apply;
  applicable->reset = ags_select_buffer_dialog_reset;
}

void
ags_select_buffer_dialog_init(AgsSelectBufferDialog *select_buffer_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  select_buffer_dialog->flags = 0;

  g_object_set(select_buffer_dialog,
	       "title", i18n("select buffers"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) select_buffer_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* copy selection */
  select_buffer_dialog->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_toggle_button_set_active(select_buffer_dialog->copy_selection,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_buffer_dialog->copy_selection),
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
  select_buffer_dialog->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										     AGS_SELECT_BUFFER_MAX_BEATS,
										     0.25);
  gtk_spin_button_set_digits(select_buffer_dialog->select_x0,
			     2);
  gtk_spin_button_set_value(select_buffer_dialog->select_x0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_buffer_dialog->select_x0),
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
  select_buffer_dialog->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										     AGS_SELECT_BUFFER_MAX_BEATS,
										     0.25);
  gtk_spin_button_set_digits(select_buffer_dialog->select_x1,
			     2);
  gtk_spin_button_set_value(select_buffer_dialog->select_x1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_buffer_dialog->select_x1),
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) select_buffer_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_select_buffer_dialog_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = AGS_SELECT_BUFFER_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) select_buffer_dialog->application_context == application_context){
	return;
      }
      
      if(select_buffer_dialog->application_context != NULL){
	g_object_unref(select_buffer_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      select_buffer_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) select_buffer_dialog->main_window == main_window){
	return;
      }

      if(select_buffer_dialog->main_window != NULL){
	g_object_unref(select_buffer_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      select_buffer_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_buffer_dialog_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = AGS_SELECT_BUFFER_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, select_buffer_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, select_buffer_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_buffer_dialog_connect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = AGS_SELECT_BUFFER_DIALOG(connectable);

  if((AGS_SELECT_BUFFER_DIALOG_CONNECTED & (select_buffer_dialog->flags)) != 0){
    return;
  }

  select_buffer_dialog->flags |= AGS_SELECT_BUFFER_DIALOG_CONNECTED;

  wave_editor = AGS_WINDOW(select_buffer_dialog->main_window)->wave_window->wave_editor;

  g_signal_connect(select_buffer_dialog, "response",
		   G_CALLBACK(ags_select_buffer_dialog_response_callback), select_buffer_dialog);
}

void
ags_select_buffer_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = AGS_SELECT_BUFFER_DIALOG(connectable);

  if((AGS_SELECT_BUFFER_DIALOG_CONNECTED & (select_buffer_dialog->flags)) == 0){
    return;
  }

  select_buffer_dialog->flags &= (~AGS_SELECT_BUFFER_DIALOG_CONNECTED);

  wave_editor = AGS_WINDOW(select_buffer_dialog->main_window)->wave_window->wave_editor;

  g_object_disconnect(G_OBJECT(select_buffer_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_select_buffer_dialog_response_callback),
		      select_buffer_dialog,
		      NULL);
}

void
ags_select_buffer_dialog_finalize(GObject *gobject)
{
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = (AgsSelectBufferDialog *) gobject;

  if(select_buffer_dialog->application_context != NULL){
    g_object_unref(select_buffer_dialog->application_context);
  }
  
  G_OBJECT_CLASS(ags_select_buffer_dialog_parent_class)->finalize(gobject);
}

void
ags_select_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_buffer_dialog_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_select_buffer_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_select_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_select_buffer_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_select_buffer_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsSelectBufferDialog.
 *
 * Returns: a new #AgsSelectBufferDialog
 *
 * Since: 2.0.0
 */
AgsSelectBufferDialog*
ags_select_buffer_dialog_new(GtkWidget *main_window)
{
  AgsSelectBufferDialog *select_buffer_dialog;

  select_buffer_dialog = (AgsSelectBufferDialog *) g_object_new(AGS_TYPE_SELECT_BUFFER_DIALOG,
								"main-window", main_window,
								NULL);

  return(select_buffer_dialog);
}
