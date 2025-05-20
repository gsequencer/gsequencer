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

#include <ags/app/editor/ags_envelope_dialog.h>
#include <ags/app/editor/ags_envelope_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_envelope_dialog_class_init(AgsEnvelopeDialogClass *envelope_dialog);
void ags_envelope_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_dialog_init(AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_envelope_dialog_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_envelope_dialog_dispose(GObject *gobject);
void ags_envelope_dialog_finalize(GObject *gobject);

gboolean ags_envelope_dialog_is_connected(AgsConnectable *connectable);
void ags_envelope_dialog_connect(AgsConnectable *connectable);
void ags_envelope_dialog_disconnect(AgsConnectable *connectable);

void ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_dialog_apply(AgsApplicable *applicable);
void ags_envelope_dialog_reset(AgsApplicable *applicable);

void ags_envelope_dialog_activate_button_callback(GtkButton *activate_button,
						  AgsEnvelopeDialog *envelope_dialog);

gboolean ags_envelope_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					       guint keyval,
					       guint keycode,
					       GdkModifierType state,
					       AgsEnvelopeDialog *envelope_dialog);
gboolean ags_envelope_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
						GdkModifierType keyval,
						AgsEnvelopeDialog *envelope_dialog);

void ags_envelope_dialog_real_response(AgsEnvelopeDialog *envelope_dialog,
				       gint response_id);

/**
 * SECTION:ags_envelope_dialog
 * @short_description: Edit envelope dialog
 * @title: AgsEnvelopeDialog
 * @section_id:
 * @include: ags/app/ags_envelope_dialog.h
 *
 * #AgsEnvelopeDialog is a composite widget to edit envelope controls
 * of selected AgsNote.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_envelope_dialog_parent_class = NULL;

static guint envelope_dialog_signals[LAST_SIGNAL];

GType
ags_envelope_dialog_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_envelope_dialog = 0;

    static const GTypeInfo ags_envelope_dialog_info = {
      sizeof (AgsEnvelopeDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						      "AgsEnvelopeDialog", &ags_envelope_dialog_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_envelope_dialog);
  }

  return(g_define_type_id__static);
}

void
ags_envelope_dialog_class_init(AgsEnvelopeDialogClass *envelope_dialog)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_envelope_dialog_parent_class = g_type_class_peek_parent(envelope_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_dialog;

  gobject->set_property = ags_envelope_dialog_set_property;
  gobject->get_property = ags_envelope_dialog_get_property;

  gobject->dispose = ags_envelope_dialog_dispose;
  gobject->finalize = ags_envelope_dialog_finalize;
  
  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine editor is assigned with"),
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* AgsEnvelopeDialog */
  envelope_dialog->response = ags_envelope_dialog_real_response;
  
  /* signals */
  /**
   * AgsEnvelopeDialog::response:
   * @envelope_dialog: the #AgsEnvelopeDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.11.0
   */
  envelope_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(envelope_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEnvelopeDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_envelope_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_envelope_dialog_is_connected;  
  connectable->connect = ags_envelope_dialog_connect;
  connectable->disconnect = ags_envelope_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_envelope_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_envelope_dialog_set_update;
  applicable->apply = ags_envelope_dialog_apply;
  applicable->reset = ags_envelope_dialog_reset;
}

void
ags_envelope_dialog_init(AgsEnvelopeDialog *envelope_dialog)
{
  GtkBox *vbox;
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  envelope_dialog->flags = 0;
  envelope_dialog->connectable_flags = 0;

  envelope_dialog->version = AGS_ENVELOPE_DIALOG_DEFAULT_VERSION;
  envelope_dialog->build_id = AGS_ENVELOPE_DIALOG_DEFAULT_BUILD_ID;

  envelope_dialog->machine = NULL;

  gtk_window_set_title((GtkWindow *) envelope_dialog,
		       i18n("envelope info"));
  gtk_window_set_hide_on_close(GTK_WINDOW(envelope_dialog),
			       TRUE);

  gtk_window_set_deletable(GTK_WINDOW(envelope_dialog),
			   TRUE);

  gtk_window_set_transient_for((GtkWindow *) envelope_dialog,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  gtk_window_set_default_size((GtkWindow *) envelope_dialog,
			      800, 600);

  g_signal_connect(envelope_dialog, "close-request",
		   G_CALLBACK(ags_envelope_dialog_close_request_callback), NULL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) envelope_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_envelope_dialog_key_pressed_callback), envelope_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_envelope_dialog_key_released_callback), envelope_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_envelope_dialog_modifiers_callback), envelope_dialog);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) envelope_dialog,
		       (GtkWidget *) vbox);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);

  envelope_dialog->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();

  gtk_widget_set_halign((GtkWidget *) notebook,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) notebook,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) notebook,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) notebook,
			 TRUE);

  gtk_box_append(vbox,
		 (GtkWidget *) notebook);

  /* envelope editor */
  envelope_dialog->envelope_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand((GtkWidget *) scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) scrolled_window,
			 TRUE);

  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("editor")));

  envelope_dialog->envelope_editor = ags_envelope_editor_new();
  gtk_scrolled_window_set_child(envelope_dialog->envelope_editor_scrolled_window,
				(GtkWidget *) envelope_dialog->envelope_editor);

  /* envelope info */
  envelope_dialog->envelope_info_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand((GtkWidget *) scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) scrolled_window,
			 TRUE);

  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("info")));

  envelope_dialog->envelope_info = ags_envelope_info_new();
  gtk_scrolled_window_set_child(envelope_dialog->envelope_info_scrolled_window,
				(GtkWidget *) envelope_dialog->envelope_info);

  envelope_dialog->pattern_envelope_scrolled_window = NULL;
  envelope_dialog->pattern_envelope = NULL;
  
  /* buttons */
  envelope_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) envelope_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) envelope_dialog->action_area);

  envelope_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(envelope_dialog->action_area,
		 (GtkWidget *) envelope_dialog->activate_button);

  g_signal_connect(envelope_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_envelope_dialog_activate_button_callback), envelope_dialog);
}

void
ags_envelope_dialog_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == envelope_dialog->machine){
	return;
      }

      if(envelope_dialog->machine != NULL){
	g_object_unref(envelope_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      envelope_dialog->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_dialog_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, envelope_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_dialog_dispose(GObject *gobject)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  if(envelope_dialog->machine != NULL){
    envelope_dialog->machine->envelope_dialog = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_envelope_dialog_parent_class)->dispose(gobject);
}

void
ags_envelope_dialog_finalize(GObject *gobject)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  if(envelope_dialog->machine != NULL){
    envelope_dialog->machine->envelope_dialog = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_envelope_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_envelope_dialog_is_connected(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;
  
  gboolean is_connected;
  
  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (envelope_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_envelope_dialog_connect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  envelope_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->envelope_editor));
  ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->pattern_envelope));
  }
}

void
ags_envelope_dialog_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  envelope_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->envelope_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->pattern_envelope));
  }
}

void
ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_envelope_dialog_apply(AgsApplicable *applicable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(applicable);

  ags_applicable_apply(AGS_APPLICABLE(envelope_dialog->envelope_editor));
  ags_applicable_apply(AGS_APPLICABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_applicable_apply(AGS_APPLICABLE(envelope_dialog->pattern_envelope));
  }
}

void
ags_envelope_dialog_reset(AgsApplicable *applicable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(applicable);

  ags_applicable_reset(AGS_APPLICABLE(envelope_dialog->envelope_editor));
  ags_applicable_reset(AGS_APPLICABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_applicable_reset(AGS_APPLICABLE(envelope_dialog->pattern_envelope));
  }
}

void
ags_envelope_dialog_activate_button_callback(GtkButton *activate_button,
					     AgsEnvelopeDialog *envelope_dialog)
{
  ags_envelope_dialog_response(envelope_dialog,
			       GTK_RESPONSE_ACCEPT);
}

gboolean
ags_envelope_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsEnvelopeDialog *envelope_dialog)
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
ags_envelope_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					  guint keyval,
					  guint keycode,
					  GdkModifierType state,
					  AgsEnvelopeDialog *envelope_dialog)
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
	ags_envelope_dialog_response(envelope_dialog,
				     GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_envelope_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				       GdkModifierType keyval,
				       AgsEnvelopeDialog *envelope_dialog)
{
  return(FALSE);
}

/**
 * ags_envelope_dialog_load_preset:
 * @envelope_dialog: the #AgsEnvelopeDialog
 * 
 * Load presets.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_dialog_load_preset(AgsEnvelopeDialog *envelope_dialog)
{
  ags_envelope_editor_load_preset(envelope_dialog->envelope_editor);

  if(envelope_dialog->pattern_envelope != NULL){
    ags_pattern_envelope_load_preset(envelope_dialog->pattern_envelope);
  }
}

/**
 * ags_envelope_dialog_add_pattern_tab:
 * @envelope_dialog: the #AgsEnvelopeDialog
 * 
 * Add pattern tab.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_dialog_add_pattern_tab(AgsEnvelopeDialog *envelope_dialog)
{
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;
  
  if(!AGS_IS_ENVELOPE_DIALOG(envelope_dialog)){
    return;
  }
  
  notebook = envelope_dialog->notebook;
  
  /* pattern envelope */
  envelope_dialog->pattern_envelope_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("pattern")));

  envelope_dialog->pattern_envelope = ags_pattern_envelope_new();
  gtk_scrolled_window_set_child(envelope_dialog->pattern_envelope_scrolled_window,
				(GtkWidget *) envelope_dialog->pattern_envelope);
}

void
ags_envelope_dialog_real_response(AgsEnvelopeDialog *envelope_dialog,
				  gint response_id)
{
  switch(response_id){
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_ACCEPT:
    {
      ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog));
      
      ags_applicable_apply(AGS_APPLICABLE(envelope_dialog));
    }
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
  case GTK_RESPONSE_REJECT:
    {
      if(envelope_dialog->machine != NULL){
	envelope_dialog->machine->envelope_dialog = NULL;
      }

      gtk_window_destroy((GtkWindow *) envelope_dialog);
    }
    break;
  }
}

/**
 * ags_envelope_dialog_response:
 * @envelope_dialog: the #AgsEnvelopeDialog
 * @response: the response
 *
 * Response @envelope_dialog due to user action.
 * 
 * Since: 6.11.0
 */
void
ags_envelope_dialog_response(AgsEnvelopeDialog *envelope_dialog,
			     gint response)
{
  g_return_if_fail(AGS_IS_ENVELOPE_DIALOG(envelope_dialog));
  
  g_object_ref((GObject *) envelope_dialog);
  g_signal_emit(G_OBJECT(envelope_dialog),
		envelope_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) envelope_dialog);
}

/**
 * ags_envelope_dialog_new:
 * @title: the title
 * @transient_for: the #GtkWindow
 * @machine: the assigned machine.
 *
 * Creates an #AgsEnvelopeDialog
 *
 * Returns: a new #AgsEnvelopeDialog
 *
 * Since: 3.0.0
 */
AgsEnvelopeDialog*
ags_envelope_dialog_new(gchar *title,
			GtkWindow *transient_for,
			AgsMachine *machine)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = (AgsEnvelopeDialog *) g_object_new(AGS_TYPE_ENVELOPE_DIALOG,
						       "title", title,
						       "transient-for", transient_for,
						       "machine", machine,
						       NULL);

  return(envelope_dialog);
}
