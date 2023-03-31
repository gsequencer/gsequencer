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

#include <ags/app/editor/ags_envelope_dialog.h>
#include <ags/app/editor/ags_envelope_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

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

void ags_envelope_dialog_connect(AgsConnectable *connectable);
void ags_envelope_dialog_disconnect(AgsConnectable *connectable);
void ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_dialog_apply(AgsApplicable *applicable);
void ags_envelope_dialog_reset(AgsApplicable *applicable);

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
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_envelope_dialog_parent_class = NULL;

GType
ags_envelope_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    ags_type_envelope_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						      "AgsEnvelopeDialog", &ags_envelope_dialog_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_dialog);
  }

  return g_define_type_id__volatile;
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
}

void
ags_envelope_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_envelope_dialog_connect;
  connectable->disconnect = ags_envelope_dialog_disconnect;
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
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;

  envelope_dialog->flags = 0;
  envelope_dialog->connectable_flags = 0;

  envelope_dialog->version = AGS_ENVELOPE_DIALOG_DEFAULT_VERSION;
  envelope_dialog->build_id = AGS_ENVELOPE_DIALOG_DEFAULT_BUILD_ID;

  envelope_dialog->machine = NULL;

  envelope_dialog->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) envelope_dialog),
		 (GtkWidget *) notebook);

  /* envelope editor */
  envelope_dialog->envelope_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("editor")));

  envelope_dialog->envelope_editor = ags_envelope_editor_new();
  gtk_scrolled_window_set_child(envelope_dialog->envelope_editor_scrolled_window,
				(GtkWidget *) envelope_dialog->envelope_editor);

  /* envelope info */
  envelope_dialog->envelope_info_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("info")));

  envelope_dialog->envelope_info = ags_envelope_info_new();
  gtk_scrolled_window_set_child(envelope_dialog->envelope_info_scrolled_window,
				(GtkWidget *) envelope_dialog->envelope_info);

  envelope_dialog->pattern_envelope_scrolled_window = NULL;
  envelope_dialog->pattern_envelope = NULL;

  gtk_window_set_default_size((GtkWindow *) envelope_dialog,
			      800, 600);
  
  /* GtkButton's in GtkDialog->action_area  */
  envelope_dialog->apply = (GtkButton *) gtk_dialog_add_button(GTK_DIALOG(envelope_dialog),
							       i18n("_Apply"),
							       GTK_RESPONSE_APPLY);
  
  envelope_dialog->ok = (GtkButton *) gtk_dialog_add_button(GTK_DIALOG(envelope_dialog),
							    i18n("_OK"),
							    GTK_RESPONSE_OK);
  
  envelope_dialog->cancel = (GtkButton *) gtk_dialog_add_button(GTK_DIALOG(envelope_dialog),
								i18n("_Cancel"),
								GTK_RESPONSE_CANCEL);
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

void
ags_envelope_dialog_connect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (envelope_dialog->connectable_flags)) != 0){
    return;
  }

  envelope_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->envelope_editor));
  ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog->pattern_envelope));
  }
  
  /* applicable */
  g_signal_connect((GObject *) envelope_dialog->apply, "clicked",
		   G_CALLBACK(ags_envelope_dialog_apply_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->ok, "clicked",
		   G_CALLBACK(ags_envelope_dialog_ok_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->cancel, "clicked",
		   G_CALLBACK(ags_envelope_dialog_cancel_callback), (gpointer) envelope_dialog);
}

void
ags_envelope_dialog_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (envelope_dialog->connectable_flags)) == 0){
    return;
  }

  envelope_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->envelope_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->envelope_info));

  if(envelope_dialog->pattern_envelope != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog->pattern_envelope));
  }
  
  /* applicable */
  g_object_disconnect((GObject *) envelope_dialog->apply,
		      "any_signal::clicked",
		      G_CALLBACK(ags_envelope_dialog_apply_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->ok,
		      "any_signal::clicked",
		      G_CALLBACK(ags_envelope_dialog_ok_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->cancel,
		      "any_signal::clicked",
		      G_CALLBACK(ags_envelope_dialog_cancel_callback),
		      (gpointer) envelope_dialog,
		      NULL);
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
