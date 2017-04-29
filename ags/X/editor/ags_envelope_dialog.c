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

#include <ags/X/editor/ags_envelope_dialog.h>
#include <ags/X/editor/ags_envelope_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>

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
void ags_envelope_dialog_connect(AgsConnectable *connectable);
void ags_envelope_dialog_disconnect(AgsConnectable *connectable);
void ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_dialog_apply(AgsApplicable *applicable);
void ags_envelope_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_envelope_dialog
 * @short_description: pack pad editors.
 * @title: AgsEnvelopeDialog
 * @section_id:
 * @include: ags/X/ags_envelope_dialog.h
 *
 * #AgsEnvelopeDialog is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editors.
 */

enum{
  SET_MACHINE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static guint envelope_dialog_signals[LAST_SIGNAL];

GType
ags_envelope_dialog_get_type(void)
{
  static GType ags_type_envelope_dialog = 0;

  if(!ags_type_envelope_dialog){
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
						      "AgsEnvelopeDialog\0", &ags_envelope_dialog_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_envelope_dialog);
}

void
ags_envelope_dialog_class_init(AgsEnvelopeDialogClass *envelope_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_dialog;

  gobject->set_property = ags_envelope_dialog_set_property;
  gobject->get_property = ags_envelope_dialog_get_property;

  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 0.8.1
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine editor is assigned with\0",
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
  gtk_window_set_title((GtkWindow *) envelope_dialog,
		       g_strdup("Envelope\0"));

  envelope_dialog->flags = 0;

  envelope_dialog->version = AGS_ENVELOPE_DIALOG_DEFAULT_VERSION;
  envelope_dialog->build_id = AGS_ENVELOPE_DIALOG_DEFAULT_BUILD_ID;

  envelope_dialog->machine = NULL;

  //TODO:JK: implement me
  
  /* GtkButton's in GtkDialog->action_area  */
  envelope_dialog->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->apply,
		     FALSE, FALSE,
		     0);
  
  envelope_dialog->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->ok,
		     FALSE, FALSE,
		     0);
  
  envelope_dialog->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->cancel,
		     FALSE, FALSE,
		     0);
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
ags_envelope_dialog_connect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_ENVELOPE_DIALOG_CONNECTED & (envelope_dialog->flags)) != 0){
    return;
  }

  envelope_dialog->flags |= AGS_ENVELOPE_DIALOG_CONNECTED;
  
  /* applicable */
  g_signal_connect((GObject *) envelope_dialog->apply, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_apply_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->ok, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_ok_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->cancel, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_cancel_callback), (gpointer) envelope_dialog);
}

void
ags_envelope_dialog_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_ENVELOPE_DIALOG_CONNECTED & (envelope_dialog->flags)) == 0){
    return;
  }

  envelope_dialog->flags &= (~AGS_ENVELOPE_DIALOG_CONNECTED);

  /* applicable */
  g_object_disconnect((GObject *) envelope_dialog->apply,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_dialog_apply_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->ok,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_dialog_ok_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->cancel,
		      "clicked\0",
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
  //TODO:JK: implement me
}

void
ags_envelope_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_envelope_dialog_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsEnvelopeDialog
 *
 * Returns: a new #AgsEnvelopeDialog
 *
 * Since: 0.8.1
 */
AgsEnvelopeDialog*
ags_envelope_dialog_new(AgsMachine *machine)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = (AgsEnvelopeDialog *) g_object_new(AGS_TYPE_ENVELOPE_DIALOG,
						       "machine\0", machine,
						       NULL);

  return(envelope_dialog);
}
