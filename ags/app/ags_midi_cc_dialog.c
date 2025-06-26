/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/ags_midi_cc_dialog.h>
#include <ags/app/ags_midi_cc_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

void ags_midi_cc_dialog_class_init(AgsMidiCCDialogClass *midi_cc_dialog);
void ags_midi_cc_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_cc_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_cc_dialog_init(AgsMidiCCDialog *midi_cc_dialog);
void ags_midi_cc_dialog_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_midi_cc_dialog_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_midi_cc_dialog_dispose(GObject *gobject);

gboolean ags_midi_cc_dialog_is_connected(AgsConnectable *connectable);
void ags_midi_cc_dialog_connect(AgsConnectable *connectable);
void ags_midi_cc_dialog_disconnect(AgsConnectable *connectable);

void ags_midi_cc_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_cc_dialog_apply(AgsApplicable *applicable);
void ags_midi_cc_dialog_reset(AgsApplicable *applicable);

void ags_midi_cc_dialog_show(GtkWidget *widget);

void ags_midi_cc_dialog_add_button_callback(GtkButton *add_button,
					    AgsMidiCCDialog *midi_cc_dialog);
void ags_midi_cc_dialog_activate_button_callback(GtkButton *activate_button,
						 AgsMidiCCDialog *midi_cc_dialog);

gboolean ags_midi_cc_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						 guint keyval,
						 guint keycode,
						 GdkModifierType state,
						 AgsMidiCCDialog *midi_cc_dialog);
void ags_midi_cc_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsMidiCCDialog *midi_cc_dialog);
gboolean ags_midi_cc_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					       GdkModifierType keyval,
					       AgsMidiCCDialog *midi_cc_dialog);

void ags_midi_cc_dialog_real_response(AgsMidiCCDialog *midi_cc_dialog,
				      gint response_id);

/**
 * SECTION:ags_midi_cc_dialog
 * @short_description: Assign MIDI CC to ags-fx ports
 * @title: AgsMidiCCDialog
 * @section_id:
 * @include: ags/app/ags_midi_cc_dialog.h
 *
 * #AgsMidiCCDialog is a composite widget to assign MIDI CC to ags-fx ports.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_midi_cc_dialog_parent_class = NULL;

static guint midi_cc_dialog_signals[LAST_SIGNAL];

GType
ags_midi_cc_dialog_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_midi_cc_dialog = 0;

    static const GTypeInfo ags_midi_cc_dialog_info = {
      sizeof (AgsMidiCCDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_cc_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiCCDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_cc_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_cc_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						     "AgsMidiCCDialog", &ags_midi_cc_dialog_info,
						     0);

    g_type_add_interface_static(ags_type_midi_cc_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_cc_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_midi_cc_dialog);
  }

  return(g_define_type_id__static);
}

void
ags_midi_cc_dialog_class_init(AgsMidiCCDialogClass *midi_cc_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_midi_cc_dialog_parent_class = g_type_class_peek_parent(midi_cc_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_cc_dialog;

  gobject->set_property = ags_midi_cc_dialog_set_property;
  gobject->get_property = ags_midi_cc_dialog_get_property;

  gobject->dispose = ags_midi_cc_dialog_dispose;
  
  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 8.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine editor is assigned with"),
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_cc_dialog;

  widget->show = ags_midi_cc_dialog_show;

  /* AgsMidiCCDialog */
  midi_cc_dialog->response = ags_midi_cc_dialog_real_response;
  
  /* signals */
  /**
   * AgsMidiCCDialog::response:
   * @midi_cc_dialog: the #AgsMidiCCDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 8.0.0
   */
  midi_cc_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(midi_cc_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiCCDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_midi_cc_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_cc_dialog_is_connected;  
  connectable->connect = ags_midi_cc_dialog_connect;
  connectable->disconnect = ags_midi_cc_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_midi_cc_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_cc_dialog_set_update;
  applicable->apply = ags_midi_cc_dialog_apply;
  applicable->reset = ags_midi_cc_dialog_reset;
}

void
ags_midi_cc_dialog_init(AgsMidiCCDialog *midi_cc_dialog)
{
  GtkBox *vbox;
  GtkLabel *label;
  GtkGrid *grid;
  GtkBox *hbox;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  midi_cc_dialog->flags = 0;
  midi_cc_dialog->connectable_flags = 0;

  midi_cc_dialog->version = AGS_MIDI_CC_DIALOG_DEFAULT_VERSION;
  midi_cc_dialog->build_id = AGS_MIDI_CC_DIALOG_DEFAULT_BUILD_ID;

  midi_cc_dialog->machine = NULL;

  gtk_window_set_title((GtkWindow *) midi_cc_dialog,
		       i18n("MIDI connection"));
  gtk_window_set_hide_on_close(GTK_WINDOW(midi_cc_dialog),
			       TRUE);

  gtk_window_set_deletable(GTK_WINDOW(midi_cc_dialog),
			   TRUE);

  g_signal_connect(midi_cc_dialog, "close-request",
		   G_CALLBACK(ags_midi_cc_dialog_close_request_callback), NULL);  

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) midi_cc_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_midi_cc_dialog_key_pressed_callback), midi_cc_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_midi_cc_dialog_key_released_callback), midi_cc_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_midi_cc_dialog_modifiers_callback), midi_cc_dialog);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) midi_cc_dialog,
		       (GtkWidget *) vbox);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) grid);

  /* midi CC editor */
  midi_cc_dialog->midi_cc_editor = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							  AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) midi_cc_dialog->midi_cc_editor);

  gtk_widget_set_valign((GtkWidget *) midi_cc_dialog->midi_cc_editor,
			GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) midi_cc_dialog->midi_cc_editor,
			 FALSE);

  /* buttons */
  midi_cc_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						       AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) midi_cc_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) midi_cc_dialog->action_area);

  /* add */
  midi_cc_dialog->add_button = (GtkButton *) gtk_button_new_from_icon_name("list-add");

  gtk_box_append(midi_cc_dialog->action_area,
		 (GtkWidget *) midi_cc_dialog->add_button);

  g_signal_connect(midi_cc_dialog->add_button, "clicked",
		   G_CALLBACK(ags_midi_cc_dialog_add_button_callback), midi_cc_dialog);

  /* activate */
  midi_cc_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(midi_cc_dialog->action_area,
		 (GtkWidget *) midi_cc_dialog->activate_button);

  g_signal_connect(midi_cc_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_midi_cc_dialog_activate_button_callback), midi_cc_dialog);
}

void
ags_midi_cc_dialog_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = AGS_MIDI_CC_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == midi_cc_dialog->machine){
	return;
      }

      if(midi_cc_dialog->machine != NULL){
	g_object_unref(midi_cc_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      midi_cc_dialog->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_cc_dialog_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = AGS_MIDI_CC_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, midi_cc_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_cc_dialog_dispose(GObject *gobject)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = AGS_MIDI_CC_DIALOG(gobject);

  if(midi_cc_dialog->machine != NULL){
    midi_cc_dialog->machine->midi_cc_dialog = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_midi_cc_dialog_parent_class)->dispose(gobject);
}

gboolean
ags_midi_cc_dialog_is_connected(AgsConnectable *connectable)
{
  AgsMidiCCDialog *midi_cc_dialog;
  
  gboolean is_connected;
  
  midi_cc_dialog = AGS_MIDI_CC_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (midi_cc_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_midi_cc_dialog_connect(AgsConnectable *connectable)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = AGS_MIDI_CC_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  midi_cc_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  //TODO:JK: implement me
}

void
ags_midi_cc_dialog_disconnect(AgsConnectable *connectable)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = AGS_MIDI_CC_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  midi_cc_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_midi_cc_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_midi_cc_dialog_apply(AgsApplicable *applicable)
{
  AgsMidiCCDialog *midi_cc_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  
  midi_cc_dialog = AGS_MIDI_CC_DIALOG(applicable);

  machine = midi_cc_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  //TODO:JK: implement me
}

void
ags_midi_cc_dialog_reset(AgsApplicable *applicable)
{
  AgsMidiCCDialog *midi_cc_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  
  midi_cc_dialog = AGS_MIDI_CC_DIALOG(applicable);

  machine = midi_cc_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  //TODO:JK: implement me
}

void
ags_midi_cc_dialog_show(GtkWidget *widget)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = (AgsMidiCCDialog *) widget;

  GTK_WIDGET_CLASS(ags_midi_cc_dialog_parent_class)->show(widget);
}

void
ags_midi_cc_dialog_add_button_callback(GtkButton *add_button,
				       AgsMidiCCDialog *midi_cc_dialog)
{
}

void
ags_midi_cc_dialog_activate_button_callback(GtkButton *activate_button,
					    AgsMidiCCDialog *midi_cc_dialog)
{
  ags_midi_cc_dialog_response(midi_cc_dialog,
			      GTK_RESPONSE_ACCEPT);
}

gboolean
ags_midi_cc_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsMidiCCDialog *midi_cc_dialog)
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
ags_midi_cc_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsMidiCCDialog *midi_cc_dialog)
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
	ags_midi_cc_dialog_response(midi_cc_dialog,
				    GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_midi_cc_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				      GdkModifierType keyval,
				      AgsMidiCCDialog *midi_cc_dialog)
{
  return(FALSE);
}

void
ags_midi_cc_dialog_real_response(AgsMidiCCDialog *midi_cc_dialog,
				 gint response_id)
{
  if(response_id == GTK_RESPONSE_OK ||
     response_id == GTK_RESPONSE_ACCEPT){
    ags_applicable_apply(AGS_APPLICABLE(midi_cc_dialog));
  }
  
  gtk_window_destroy((GtkWindow *) midi_cc_dialog);
}

/**
 * ags_midi_cc_dialog_response:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * @response: the response
 *
 * Response @midi_cc_dialog due to user action.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_dialog_response(AgsMidiCCDialog *midi_cc_dialog,
			    gint response)
{
  g_return_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog));
  
  g_object_ref((GObject *) midi_cc_dialog);
  g_signal_emit(G_OBJECT(midi_cc_dialog),
		midi_cc_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) midi_cc_dialog);
}

/**
 * ags_midi_cc_dialog_new:
 * @transient_for: the transient for window
 * @machine: the assigned machine.
 *
 * Creates an #AgsMidiCCDialog
 *
 * Returns: a new #AgsMidiCCDialog
 *
 * Since: 8.0.0
 */
AgsMidiCCDialog*
ags_midi_cc_dialog_new(GtkWindow *transient_for,
		       AgsMachine *machine)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = (AgsMidiCCDialog *) g_object_new(AGS_TYPE_MIDI_CC_DIALOG,
						    "transient-for", transient_for,
						    "machine", machine,
						    NULL);

  return(midi_cc_dialog);
}
