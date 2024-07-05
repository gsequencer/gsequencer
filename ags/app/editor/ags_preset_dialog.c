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

#include <ags/app/editor/ags_preset_dialog.h>
#include <ags/app/editor/ags_preset_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_preset_dialog_class_init(AgsPresetDialogClass *preset_dialog);
void ags_preset_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_preset_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_preset_dialog_init(AgsPresetDialog *preset_dialog);
void ags_preset_dialog_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_preset_dialog_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_preset_dialog_dispose(GObject *gobject);
void ags_preset_dialog_finalize(GObject *gobject);

gboolean ags_preset_dialog_is_connected(AgsConnectable *connectable);
void ags_preset_dialog_connect(AgsConnectable *connectable);
void ags_preset_dialog_disconnect(AgsConnectable *connectable);

void ags_preset_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_preset_dialog_apply(AgsApplicable *applicable);
void ags_preset_dialog_reset(AgsApplicable *applicable);

void ags_preset_dialog_activate_button_callback(GtkButton *activate_button,
						AgsPresetDialog *preset_dialog);

gboolean ags_preset_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						guint keyval,
						guint keycode,
						GdkModifierType state,
						AgsPresetDialog *preset_dialog);
void ags_preset_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsPresetDialog *preset_dialog);
gboolean ags_preset_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					      GdkModifierType keyval,
					      AgsPresetDialog *preset_dialog);

void ags_preset_dialog_real_response(AgsPresetDialog *preset_dialog,
				     gint response_id);

/**
 * SECTION:ags_preset_dialog
 * @short_description: Edit preset dialog
 * @title: AgsPresetDialog
 * @section_id:
 * @include: ags/app/ags_preset_dialog.h
 *
 * #AgsPresetDialog is a composite widget to edit preset controls
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

static gpointer ags_preset_dialog_parent_class = NULL;

static guint preset_dialog_signals[LAST_SIGNAL];

GType
ags_preset_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_preset_dialog = 0;

    static const GTypeInfo ags_preset_dialog_info = {
      sizeof (AgsPresetDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_preset_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPresetDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_preset_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_preset_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_preset_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_preset_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						    "AgsPresetDialog", &ags_preset_dialog_info,
						    0);

    g_type_add_interface_static(ags_type_preset_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_preset_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_preset_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_preset_dialog_class_init(AgsPresetDialogClass *preset_dialog)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_preset_dialog_parent_class = g_type_class_peek_parent(preset_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) preset_dialog;

  gobject->set_property = ags_preset_dialog_set_property;
  gobject->get_property = ags_preset_dialog_get_property;

  gobject->dispose = ags_preset_dialog_dispose;
  gobject->finalize = ags_preset_dialog_finalize;
  
  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 4.1.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine editor is assigned with"),
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* AgsPresetDialog */
  preset_dialog->response = ags_preset_dialog_real_response;
  
  /* signals */
  /**
   * AgsPresetDialog::response:
   * @preset_dialog: the #AgsPresetDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.11.0
   */
  preset_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(preset_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPresetDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_preset_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_preset_dialog_is_connected;  
  connectable->connect = ags_preset_dialog_connect;
  connectable->disconnect = ags_preset_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_preset_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_preset_dialog_set_update;
  applicable->apply = ags_preset_dialog_apply;
  applicable->reset = ags_preset_dialog_reset;
}

void
ags_preset_dialog_init(AgsPresetDialog *preset_dialog)
{
  GtkBox *vbox;
  GtkScrolledWindow *scrolled_window;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  preset_dialog->flags = 0;
  preset_dialog->connectable_flags = 0;

  preset_dialog->version = AGS_PRESET_DIALOG_DEFAULT_VERSION;
  preset_dialog->build_id = AGS_PRESET_DIALOG_DEFAULT_BUILD_ID;

  preset_dialog->machine = NULL;

  gtk_window_set_title((GtkWindow *) preset_dialog,
		       i18n("preset editor"));

  gtk_window_set_deletable(GTK_WINDOW(preset_dialog),
			   TRUE);

  gtk_window_set_transient_for((GtkWindow *) preset_dialog,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  gtk_window_set_default_size((GtkWindow *) preset_dialog,
			      1024, 800);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) preset_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_preset_dialog_key_pressed_callback), preset_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_preset_dialog_key_released_callback), preset_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_preset_dialog_modifiers_callback), preset_dialog);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) preset_dialog,
		       (GtkWidget *) vbox);

  gtk_widget_set_valign(vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(vbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand(vbox,
			 TRUE);
  gtk_widget_set_vexpand(vbox,
			 TRUE);
  
  /* preset editor */
  scrolled_window = 
    preset_dialog->preset_editor_scrolled_window =(GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand((GtkWidget *) scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) scrolled_window,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);
  
  gtk_box_append(vbox,
		 (GtkWidget *) scrolled_window);

  preset_dialog->preset_editor = ags_preset_editor_new();
  gtk_scrolled_window_set_child(preset_dialog->preset_editor_scrolled_window,
				(GtkWidget *) preset_dialog->preset_editor);
  
  /* buttons */
  preset_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign(preset_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) preset_dialog->action_area);

  preset_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(preset_dialog->action_area,
		 (GtkWidget *) preset_dialog->activate_button);

  g_signal_connect(preset_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_preset_dialog_activate_button_callback), preset_dialog);
}

void
ags_preset_dialog_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == preset_dialog->machine){
	return;
      }

      if(preset_dialog->machine != NULL){
	g_object_unref(preset_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      preset_dialog->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_preset_dialog_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value,
			 preset_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_preset_dialog_dispose(GObject *gobject)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(gobject);

  if(preset_dialog->machine != NULL){
    preset_dialog->machine->preset_dialog = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_preset_dialog_parent_class)->dispose(gobject);
}

void
ags_preset_dialog_finalize(GObject *gobject)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(gobject);

  if(preset_dialog->machine != NULL){
    preset_dialog->machine->preset_dialog = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_preset_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_preset_dialog_is_connected(AgsConnectable *connectable)
{
  AgsPresetDialog *preset_dialog;
  
  gboolean is_connected;
  
  preset_dialog = AGS_PRESET_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (preset_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_preset_dialog_connect(AgsConnectable *connectable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  preset_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(preset_dialog->preset_editor));
}

void
ags_preset_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  preset_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(preset_dialog->preset_editor));
}

void
ags_preset_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_preset_dialog_apply(AgsApplicable *applicable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(applicable);

  ags_applicable_apply(AGS_APPLICABLE(preset_dialog->preset_editor));
}

void
ags_preset_dialog_reset(AgsApplicable *applicable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(applicable);

  ags_applicable_reset(AGS_APPLICABLE(preset_dialog->preset_editor));
}

void
ags_preset_dialog_activate_button_callback(GtkButton *activate_button,
					   AgsPresetDialog *preset_dialog)
{
  ags_preset_dialog_response(preset_dialog,
			     GTK_RESPONSE_ACCEPT);
}

gboolean
ags_preset_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
				       guint keyval,
				       guint keycode,
				       GdkModifierType state,
				       AgsPresetDialog *preset_dialog)
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
ags_preset_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsPresetDialog *preset_dialog)
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
	ags_preset_dialog_response(preset_dialog,
				   GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_preset_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				     GdkModifierType keyval,
				     AgsPresetDialog *preset_dialog)
{
  return(FALSE);
}

void
ags_preset_dialog_real_response(AgsPresetDialog *preset_dialog,
				gint response_id)
{
  switch(response_id){
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_ACCEPT:
    {
      ags_connectable_disconnect(AGS_CONNECTABLE(preset_dialog));
      
      ags_applicable_apply(AGS_APPLICABLE(preset_dialog));
    }
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
  case GTK_RESPONSE_REJECT:
    {
      if(preset_dialog->machine != NULL){
	preset_dialog->machine->preset_dialog = NULL;
      }
	
      gtk_window_destroy((GtkWindow *) preset_dialog);
    }
    break;
  }
}

/**
 * ags_preset_dialog_response:
 * @preset_dialog: the #AgsPresetDialog
 * @response: the response
 *
 * Response @preset_dialog due to user action.
 * 
 * Since: 6.11.0
 */
void
ags_preset_dialog_response(AgsPresetDialog *preset_dialog,
			   gint response)
{
  g_return_if_fail(AGS_IS_PRESET_DIALOG(preset_dialog));
  
  g_object_ref((GObject *) preset_dialog);
  g_signal_emit(G_OBJECT(preset_dialog),
		preset_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) preset_dialog);
}

/**
 * ags_preset_dialog_new:
 * @title: the title
 * @transient_for: the #GtkWindow
 * @machine: the assigned machine.
 *
 * Creates an #AgsPresetDialog
 *
 * Returns: a new #AgsPresetDialog
 *
 * Since: 4.1.0
 */
AgsPresetDialog*
ags_preset_dialog_new(gchar *title,
		      GtkWindow *transient_for,
		      AgsMachine *machine)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = (AgsPresetDialog *) g_object_new(AGS_TYPE_PRESET_DIALOG,
						   "title", title,
						   "transient-for", transient_for,
						   "machine", machine,
						   NULL);

  return(preset_dialog);
}
