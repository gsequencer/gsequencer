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

#include <ags/ags_api_config.h>

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

static const gchar* const control_change_strv[] = {
  "bank select",
  "modulation wheel",
  "breath controller",
  "foot controller",
  "portamento time",
  "channel volume",
  "balance",
  "pan",
  "expression controller",
  "effect control 1",
  "effect control 2",
  "general purpose controller 1",
  "general purpose controller 2",
  "general purpose controller 3",
  "general purpose controller 4",
  "change program",
  "change pressure",
  "pitch bend",
  "MIDI v2.0 change program",
  "MIDI v2.0 change pressure",
  "MIDI v2.0 pitch bend",
  NULL,
};

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
  GtkScrolledWindow *scrolled_window;
  GtkBox *vbox;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
#if defined(AGS_OSXAPI)
  midi_cc_dialog->flags = AGS_MIDI_CC_DIALOG_SHOW_MIDI_2_0;
#else
  midi_cc_dialog->flags = AGS_MIDI_CC_DIALOG_SHOW_MIDI_1_0;
#endif
  
  midi_cc_dialog->connectable_flags = 0;

  midi_cc_dialog->version = AGS_MIDI_CC_DIALOG_DEFAULT_VERSION;
  midi_cc_dialog->build_id = AGS_MIDI_CC_DIALOG_DEFAULT_BUILD_ID;

  midi_cc_dialog->machine = NULL;

  gtk_window_set_title((GtkWindow *) midi_cc_dialog,
		       i18n("MIDI CC connection"));
  gtk_window_set_hide_on_close(GTK_WINDOW(midi_cc_dialog),
			       TRUE);

  gtk_window_set_deletable(GTK_WINDOW(midi_cc_dialog),
			   TRUE);

  gtk_window_set_default_size((GtkWindow *) midi_cc_dialog,
			      800, 600);

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
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);

  /* scrolled window */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

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
  
  /* midi CC editor */
  midi_cc_dialog->editor = NULL;

  midi_cc_dialog->editor_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) midi_cc_dialog->editor_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) midi_cc_dialog->editor_box,
			 TRUE);

  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) midi_cc_dialog->editor_box);
  
  /* buttons */
  midi_cc_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						       AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) midi_cc_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) midi_cc_dialog->action_area);

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
  
  xmlNode *node;
  
  GList *start_dialog_model, *dialog_model;
  GList *start_editor, *editor;

  gint i, i_stop;
  
  midi_cc_dialog = AGS_MIDI_CC_DIALOG(applicable);

  machine = midi_cc_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  /* editor */
  editor =
    start_editor = ags_midi_cc_dialog_get_editor(midi_cc_dialog);

  while(editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(editor->data));

    /* iterate */
    editor = editor->next;
  }
  
  /* model */
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;
  
  dialog_model = ags_machine_find_dialog_model(machine,
					       dialog_model,
					       "ags-midi-cc-dialog",
					       NULL,
					       NULL);

  if(dialog_model != NULL){
    node = dialog_model->data;
  }

  if(node != NULL){
    ags_machine_remove_dialog_model(machine,
				    node);
  }

  node = ags_midi_cc_dialog_to_xml_node(midi_cc_dialog);
  ags_machine_add_dialog_model(machine,
			       node);
  
  /* free */
  g_list_free(start_editor);
  g_list_free(start_dialog_model);
}

void
ags_midi_cc_dialog_reset(AgsApplicable *applicable)
{
  AgsMidiCCDialog *midi_cc_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  
  xmlNode *node;
  
  GList *start_editor, *editor;
  GList *start_dialog_model, *dialog_model;
  
  midi_cc_dialog = AGS_MIDI_CC_DIALOG(applicable);

  machine = midi_cc_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  if(midi_cc_dialog->editor == NULL){
    ags_midi_cc_dialog_load_editor(midi_cc_dialog);
  }
  
  /* editor */
  editor =
    start_editor = ags_midi_cc_dialog_get_editor(midi_cc_dialog);

  while(editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(editor->data));

    /* iterate */
    editor = editor->next;
  }

  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  dialog_model = ags_machine_find_dialog_model(machine,
					       dialog_model,
					       "ags-midi-cc-dialog",
					       NULL,
					       NULL);

  if(dialog_model != NULL){
    node = dialog_model->data;
  }
  
  if(node != NULL){
    ags_midi_cc_dialog_from_xml_node(midi_cc_dialog,
				     node);
  }
  
  /* free */
  g_list_free(start_editor);
  g_list_free(start_dialog_model);
}

void
ags_midi_cc_dialog_show(GtkWidget *widget)
{
  AgsMidiCCDialog *midi_cc_dialog;

  midi_cc_dialog = (AgsMidiCCDialog *) widget;

  GTK_WIDGET_CLASS(ags_midi_cc_dialog_parent_class)->show(widget);
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
 * ags_midi_cc_dialog_get_editor:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * 
 * Get editor.
 * 
 * Returns: (transfer container): the #GList-struct containig #AgsMidiCCEditor
 * 
 * Since: 8.0.0
 */
GList*
ags_midi_cc_dialog_get_editor(AgsMidiCCDialog *midi_cc_dialog)
{
  g_return_val_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog), NULL);

  return(g_list_reverse(g_list_copy(midi_cc_dialog->editor)));
}

/**%
 * ags_midi_cc_dialog_add_editor:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * @editor: the #AgsMidiCCEditor
 * 
 * Add @editor to @midi_cc_dialog.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_dialog_add_editor(AgsMidiCCDialog *midi_cc_dialog,
			      AgsMidiCCEditor *editor)
{
  g_return_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog));
  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(editor));

  if(g_list_find(midi_cc_dialog->editor, editor) == NULL){
    editor->parent_midi_cc_dialog = (GtkWidget *) midi_cc_dialog;

    midi_cc_dialog->editor = g_list_prepend(midi_cc_dialog->editor,
					    editor);
    
    gtk_box_append(midi_cc_dialog->editor_box,
		   (GtkWidget *) editor);
  }
}

/**
 * ags_midi_cc_dialog_remove_editor:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * @editor: the #AgsMidiCCEditor
 * 
 * Remove @editor from @midi_cc_dialog.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_dialog_remove_editor(AgsMidiCCDialog *midi_cc_dialog,
				 AgsMidiCCEditor *editor)
{
  g_return_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog));
  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(editor));

  if(g_list_find(midi_cc_dialog->editor, editor) != NULL){
    editor->parent_midi_cc_dialog = NULL;
    
    midi_cc_dialog->editor = g_list_remove(midi_cc_dialog->editor,
					   editor);
    
    gtk_box_remove(midi_cc_dialog->editor_box,
		   (GtkWidget *) editor);
  }
}

/**
 * ags_midi_cc_editor_load_editor:
 * @midi_cc_editor: the #AgsMidiCCEditor
 *
 * Load editor.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_dialog_load_editor(AgsMidiCCDialog *midi_cc_dialog)
{
  AgsMidiCCEditor *midi_cc_editor;

  gchar **iter;

  AgsRecallMidi2ControlChange *cc_iter;

  static const AgsRecallMidi2ControlChange control_change_enum[] = {
    AGS_RECALL_MIDI2_MIDI1_BANK_SELECT,
    AGS_RECALL_MIDI2_MIDI1_MODULATION_WHEEL,
    AGS_RECALL_MIDI2_MIDI1_BREATH_CONTROLLER,
    AGS_RECALL_MIDI2_MIDI1_FOOT_CONTROLLER,
    AGS_RECALL_MIDI2_MIDI1_PORTAMENTO_TIME,
    AGS_RECALL_MIDI2_MIDI1_CHANNEL_VOLUME,
    AGS_RECALL_MIDI2_MIDI1_BALANCE,
    AGS_RECALL_MIDI2_MIDI1_PAN,
    AGS_RECALL_MIDI2_MIDI1_EXPRESSION_CONTROLLER,
    AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_1,
    AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_2,
    AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_1,
    AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_2,
    AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_3,
    AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_4,
    AGS_RECALL_MIDI2_MIDI1_CHANGE_PROGRAM,
    AGS_RECALL_MIDI2_MIDI1_CHANGE_PRESSURE,
    AGS_RECALL_MIDI2_MIDI1_PITCH_BEND,
    AGS_RECALL_MIDI2_CHANGE_PROGRAM,
    AGS_RECALL_MIDI2_CHANGE_PRESSURE,
    AGS_RECALL_MIDI2_PITCH_BEND,
    -1,
  };
    
  g_return_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog));

  if(control_change_strv != NULL){
    for(iter = (gchar **) control_change_strv, cc_iter = control_change_enum; iter[0] != NULL; iter++, cc_iter++){
      midi_cc_editor = ags_midi_cc_editor_new();
      
      midi_cc_editor->midi2_control = cc_iter[0];
      gtk_label_set_label(midi_cc_editor->midi2_control_label,
			  iter[0]);
      
      ags_midi_cc_dialog_add_editor(midi_cc_dialog,
				    midi_cc_editor);
      
      ags_midi_cc_editor_load_port(midi_cc_editor);
    }
  }
}

/**
 * ags_midi_cc_dialog_to_xml_node:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * 
 * Serialize @midi_cc_dialog to #xmlNode-struct.
 * 
 * Returns: the serialized #xmlNode-struct
 * 
 * Since: 8.0.0
 */
xmlNode*
ags_midi_cc_dialog_to_xml_node(AgsMidiCCDialog *midi_cc_dialog)
{
  xmlNode *node;
  xmlNode *child;

  GList *start_editor, *editor;
  
  g_return_val_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog), NULL);
  
  node = xmlNewNode(NULL,
		    BAD_CAST "ags-midi-cc-dialog");

  /* editor */
  editor =
    start_editor = ags_midi_cc_dialog_get_editor(midi_cc_dialog);

  while(editor != NULL){
    GObject *gobject;

    gchar *str;

    gint val;
    
    child = xmlNewNode(NULL,
		       "editor");

    /* control */
    str = gtk_label_get_text(AGS_MIDI_CC_EDITOR(editor->data)->midi2_control_label);

    xmlNewProp(child,
	       "control",
	       str);

    /* port */
    gobject = gtk_drop_down_get_selected_item(AGS_MIDI_CC_EDITOR(editor->data)->midi2_port_drop_down);
    
    str = gtk_string_object_get_string((GtkStringObject *) gobject);

    xmlNewProp(child,
	       "port",
	       str);

    /* MIDI group */
    val = gtk_spin_button_get_value_as_int(AGS_MIDI_CC_EDITOR(editor->data)->midi2_group);

    str = g_strdup_printf("%d",
			  val);

    xmlNewProp(child,
	       "midi-group",
	       str);

    g_free(str);

    /* MIDI channel */
    val = gtk_spin_button_get_value_as_int(AGS_MIDI_CC_EDITOR(editor->data)->midi2_channel);

    str = g_strdup_printf("%d",
			  val);

    xmlNewProp(child,
	       "midi-channel",
	       str);

    g_free(str);

    /* MIDI note */
    val = gtk_spin_button_get_value_as_int(AGS_MIDI_CC_EDITOR(editor->data)->midi2_note);

    str = g_strdup_printf("%d",
			  val);

    xmlNewProp(child,
	       "midi-note",
	       str);

    g_free(str);
    
    /* add child */
    xmlAddChild(node,
		child);

    /* iterate */
    editor = editor->next;
  }

  g_list_free(start_editor);

  return(node);
}

/**
 * ags_midi_cc_dialog_from_xml_node:
 * @midi_cc_dialog: the #AgsMidiCCDialog
 * @node: the #xmlNode-struct
 * 
 * Parse @node and apply to @midi_cc_dialog.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_dialog_from_xml_node(AgsMidiCCDialog *midi_cc_dialog,
				 xmlNode *node)
{
  xmlNode *child;

  GList *start_editor, *editor;

  g_return_if_fail(AGS_IS_MIDI_CC_DIALOG(midi_cc_dialog));
  g_return_if_fail(node != NULL);

  /* editor */
  editor =
    start_editor = ags_midi_cc_dialog_get_editor(midi_cc_dialog);
  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(BAD_CAST "editor",
		     child->name,
		     7)){
	xmlChar *control;
	xmlChar *port;
	xmlChar *midi2_group;
	xmlChar *midi2_channel;
	xmlChar *midi2_note;
	
	control = xmlGetProp(child,
			     BAD_CAST "control");
	
	port = xmlGetProp(child,
			  BAD_CAST "port");

	editor = ags_midi_cc_editor_find_midi2_control(start_editor,
						       control);

	if(editor != NULL){
	  gint position;

	  position = ags_strv_index(AGS_MIDI_CC_EDITOR(editor->data)->port,
				    port);

	  if(position >= 0){
	    gtk_drop_down_set_selected(AGS_MIDI_CC_EDITOR(editor->data)->midi2_port_drop_down,
				       (guint) position);
	  }
	}
	
	xmlFree(control);
	xmlFree(port);

	/* MIDI group */
	midi2_group = xmlGetProp(child,
				BAD_CAST "midi-group");
	gtk_spin_button_set_value(AGS_MIDI_CC_EDITOR(editor->data)->midi2_group,
				  g_ascii_strtod(midi2_group,
						 NULL));
	
	/* MIDI channel */
	midi2_channel = xmlGetProp(child,
				BAD_CAST "midi-channel");
	gtk_spin_button_set_value(AGS_MIDI_CC_EDITOR(editor->data)->midi2_channel,
				  g_ascii_strtod(midi2_channel,
						 NULL));

	/* MIDI note */
	midi2_note = xmlGetProp(child,
				BAD_CAST "midi-note");
	gtk_spin_button_set_value(AGS_MIDI_CC_EDITOR(editor->data)->midi2_note,
				  g_ascii_strtod(midi2_note,
						 NULL));

	xmlFree(midi2_group);
	xmlFree(midi2_channel);
	xmlFree(midi2_note);
      }
    }

    child = child->next;
  }
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
