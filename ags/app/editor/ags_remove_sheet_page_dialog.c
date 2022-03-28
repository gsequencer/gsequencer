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

#include <ags/app/editor/ags_remove_sheet_page_dialog.h>
#include <ags/app/editor/ags_remove_sheet_page_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_sheet_edit.h>

#include <ags/i18n.h>

void ags_remove_sheet_page_dialog_class_init(AgsRemoveSheetPageDialogClass *remove_sheet_page_dialog);
void ags_remove_sheet_page_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_sheet_page_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_remove_sheet_page_dialog_init(AgsRemoveSheetPageDialog *remove_sheet_page_dialog);
void ags_remove_sheet_page_dialog_finalize(GObject *gobject);

void ags_remove_sheet_page_dialog_connect(AgsConnectable *connectable);
void ags_remove_sheet_page_dialog_disconnect(AgsConnectable *connectable);

void ags_remove_sheet_page_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_remove_sheet_page_dialog_apply(AgsApplicable *applicable);
void ags_remove_sheet_page_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_remove_sheet_page_dialog
 * @short_description: remove tool
 * @title: AgsRemoveSheetPageDialog
 * @section_id:
 * @include: ags/app/editor/ags_remove_sheet_page_dialog.h
 *
 * The #AgsRemoveSheetPageDialog lets you remove sheet editor page.
 */

static gpointer ags_remove_sheet_page_dialog_parent_class = NULL;

GType
ags_remove_sheet_page_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_remove_sheet_page_dialog = 0;

    static const GTypeInfo ags_remove_sheet_page_dialog_info = {
      sizeof (AgsRemoveSheetPageDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_sheet_page_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveSheetPageDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_sheet_page_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_sheet_page_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_remove_sheet_page_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_sheet_page_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							       "AgsRemoveSheetPageDialog", &ags_remove_sheet_page_dialog_info,
							       0);
    
    g_type_add_interface_static(ags_type_remove_sheet_page_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_remove_sheet_page_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_remove_sheet_page_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_remove_sheet_page_dialog_class_init(AgsRemoveSheetPageDialogClass *remove_sheet_page_dialog)
{
  GObjectClass *gobject;

  ags_remove_sheet_page_dialog_parent_class = g_type_class_peek_parent(remove_sheet_page_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) remove_sheet_page_dialog;

  gobject->finalize = ags_remove_sheet_page_dialog_finalize;
}

void
ags_remove_sheet_page_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_remove_sheet_page_dialog_connect;
  connectable->disconnect = ags_remove_sheet_page_dialog_disconnect;
}

void
ags_remove_sheet_page_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_remove_sheet_page_dialog_set_update;
  applicable->apply = ags_remove_sheet_page_dialog_apply;
  applicable->reset = ags_remove_sheet_page_dialog_reset;
}

void
ags_remove_sheet_page_dialog_init(AgsRemoveSheetPageDialog *remove_sheet_page_dialog)
{
  remove_sheet_page_dialog->connectable_flags = 0;

  g_object_set(remove_sheet_page_dialog,
	       "title", i18n("remove sheet page"),
	       NULL);

  gtk_window_set_hide_on_close(remove_sheet_page_dialog,
			       TRUE);
  
  //TODO:JK: implement me
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) remove_sheet_page_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_remove_sheet_page_dialog_connect(AgsConnectable *connectable)
{
  AgsRemoveSheetPageDialog *remove_sheet_page_dialog;

  remove_sheet_page_dialog = AGS_REMOVE_SHEET_PAGE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (remove_sheet_page_dialog->connectable_flags)) != 0){
    return;
  }

  remove_sheet_page_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(remove_sheet_page_dialog, "response",
		   G_CALLBACK(ags_remove_sheet_page_dialog_response_callback), remove_sheet_page_dialog);
}

void
ags_remove_sheet_page_dialog_disconnect(AgsConnectable *connectable)
{
  AgsRemoveSheetPageDialog *remove_sheet_page_dialog;

  remove_sheet_page_dialog = AGS_REMOVE_SHEET_PAGE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (remove_sheet_page_dialog->connectable_flags)) == 0){
    return;
  }

  remove_sheet_page_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(remove_sheet_page_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_remove_sheet_page_dialog_response_callback),
		      remove_sheet_page_dialog,
		      NULL);
}

void
ags_remove_sheet_page_dialog_finalize(GObject *gobject)
{
  AgsRemoveSheetPageDialog *remove_sheet_page_dialog;

  remove_sheet_page_dialog = (AgsRemoveSheetPageDialog *) gobject;
  
  G_OBJECT_CLASS(ags_remove_sheet_page_dialog_parent_class)->finalize(gobject);
}

void
ags_remove_sheet_page_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_remove_sheet_page_dialog_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_remove_sheet_page_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_remove_sheet_page_dialog_new:
 *
 * Create a new #AgsRemoveSheetPageDialog.
 *
 * Returns: a new #AgsRemoveSheetPageDialog
 *
 * Since: 3.18.0
 */
AgsRemoveSheetPageDialog*
ags_remove_sheet_page_dialog_new()
{
  AgsRemoveSheetPageDialog *remove_sheet_page_dialog;

  remove_sheet_page_dialog = (AgsRemoveSheetPageDialog *) g_object_new(AGS_TYPE_REMOVE_SHEET_PAGE_DIALOG,
								       NULL);

  return(remove_sheet_page_dialog);
}
