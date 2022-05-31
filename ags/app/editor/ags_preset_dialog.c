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

#include <ags/app/editor/ags_preset_dialog.h>
#include <ags/app/editor/ags_preset_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

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

void ags_preset_dialog_connect(AgsConnectable *connectable);
void ags_preset_dialog_disconnect(AgsConnectable *connectable);
void ags_preset_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_preset_dialog_apply(AgsApplicable *applicable);
void ags_preset_dialog_reset(AgsApplicable *applicable);

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
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_preset_dialog_parent_class = NULL;

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

    ags_type_preset_dialog = g_type_register_static(GTK_TYPE_DIALOG,
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
}

void
ags_preset_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_preset_dialog_connect;
  connectable->disconnect = ags_preset_dialog_disconnect;
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
  GtkScrolledWindow *scrolled_window;

  preset_dialog->flags = 0;
  preset_dialog->connectable_flags = 0;

  preset_dialog->version = AGS_PRESET_DIALOG_DEFAULT_VERSION;
  preset_dialog->build_id = AGS_PRESET_DIALOG_DEFAULT_BUILD_ID;

  preset_dialog->machine = NULL;

  /* preset editor */
  preset_dialog->preset_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area(preset_dialog),
		 (GtkWidget *) scrolled_window);

  preset_dialog->preset_editor = ags_preset_editor_new();
  gtk_scrolled_window_set_child(preset_dialog->preset_editor_scrolled_window,
				(GtkWidget *) preset_dialog->preset_editor);

  /* GtkButton's in GtkDialog->action_area  */
  preset_dialog->ok = (GtkButton *) gtk_dialog_add_button(GTK_DIALOG(preset_dialog),
							  i18n("_OK"),
							  GTK_RESPONSE_OK);
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
      g_value_set_object(value, preset_dialog->machine);
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

void
ags_preset_dialog_connect(AgsConnectable *connectable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_dialog->connectable_flags)) != 0){
    return;
  }

  preset_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(preset_dialog->preset_editor));
  
  /* applicable */
  g_signal_connect((GObject *) preset_dialog->ok, "clicked",
		   G_CALLBACK(ags_preset_dialog_ok_callback), (gpointer) preset_dialog);
}

void
ags_preset_dialog_disconnect(AgsConnectable *connectable)
{
  AgsPresetDialog *preset_dialog;

  preset_dialog = AGS_PRESET_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_dialog->connectable_flags)) == 0){
    return;
  }

  preset_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(preset_dialog->preset_editor));
  
  /* applicable */
  g_object_disconnect((GObject *) preset_dialog->ok,
		      "any_signal::clicked",
		      G_CALLBACK(ags_preset_dialog_ok_callback),
		      (gpointer) preset_dialog,
		      NULL);
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
