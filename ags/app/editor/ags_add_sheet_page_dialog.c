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

#include <ags/app/editor/ags_add_sheet_page_dialog.h>
#include <ags/app/editor/ags_add_sheet_page_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_sheet_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_add_sheet_page_dialog_class_init(AgsAddSheetPageDialogClass *add_sheet_page_dialog);
void ags_add_sheet_page_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_sheet_page_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_add_sheet_page_dialog_init(AgsAddSheetPageDialog *add_sheet_page_dialog);
void ags_add_sheet_page_dialog_finalize(GObject *gobject);
void ags_add_sheet_page_dialog_connect(AgsConnectable *connectable);
void ags_add_sheet_page_dialog_disconnect(AgsConnectable *connectable);
void ags_add_sheet_page_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_add_sheet_page_dialog_apply(AgsApplicable *applicable);
void ags_add_sheet_page_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_add_sheet_page_dialog
 * @short_description: add tool
 * @title: AgsAddSheetPageDialog
 * @section_id:
 * @include: ags/app/editor/ags_add_sheet_page_dialog.h
 *
 * The #AgsAddSheetPageDialog lets you add sheet editor page.
 */

static gpointer ags_add_sheet_page_dialog_parent_class = NULL;

GType
ags_add_sheet_page_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_add_sheet_page_dialog = 0;

    static const GTypeInfo ags_add_sheet_page_dialog_info = {
      sizeof (AgsAddSheetPageDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_sheet_page_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddSheetPageDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_sheet_page_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_sheet_page_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_add_sheet_page_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_sheet_page_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							    "AgsAddSheetPageDialog", &ags_add_sheet_page_dialog_info,
							    0);
    
    g_type_add_interface_static(ags_type_add_sheet_page_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_add_sheet_page_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_add_sheet_page_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_add_sheet_page_dialog_class_init(AgsAddSheetPageDialogClass *add_sheet_page_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_add_sheet_page_dialog_parent_class = g_type_class_peek_parent(add_sheet_page_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) add_sheet_page_dialog;

  gobject->finalize = ags_add_sheet_page_dialog_finalize;
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) add_sheet_page_dialog;
}

void
ags_add_sheet_page_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_add_sheet_page_dialog_connect;
  connectable->disconnect = ags_add_sheet_page_dialog_disconnect;
}

void
ags_add_sheet_page_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_add_sheet_page_dialog_set_update;
  applicable->apply = ags_add_sheet_page_dialog_apply;
  applicable->reset = ags_add_sheet_page_dialog_reset;
}

void
ags_add_sheet_page_dialog_init(AgsAddSheetPageDialog *add_sheet_page_dialog)
{
  GtkGrid *grid;
  GtkLabel *label;
  
  add_sheet_page_dialog->connectable_flags = 0;

  g_object_set(add_sheet_page_dialog,
	       "title", i18n("Add sheet page"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) add_sheet_page_dialog,
			       TRUE);
  
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) add_sheet_page_dialog),
		 (GtkWidget *) grid);  

  label = (GtkLabel *) gtk_label_new(i18n("sheet title"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  add_sheet_page_dialog->sheet_title = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) add_sheet_page_dialog->sheet_title,
		  1, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("notation x0"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  add_sheet_page_dialog->notation_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											(gdouble) AGS_ADD_SHEET_PAGE_DIALOG_MAX_NOTATION_X,
											1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) add_sheet_page_dialog->notation_x0,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("notation x1"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  add_sheet_page_dialog->notation_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											(gdouble) AGS_ADD_SHEET_PAGE_DIALOG_MAX_NOTATION_X,
											1.0);
  gtk_spin_button_set_value(add_sheet_page_dialog->notation_x1,
			    64.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) add_sheet_page_dialog->notation_x1,
		  1, 2,
		  1, 1);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) add_sheet_page_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_add_sheet_page_dialog_connect(AgsConnectable *connectable)
{
  AgsAddSheetPageDialog *add_sheet_page_dialog;

  add_sheet_page_dialog = AGS_ADD_SHEET_PAGE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (add_sheet_page_dialog->connectable_flags)) != 0){
    return;
  }

  add_sheet_page_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(add_sheet_page_dialog, "response",
		   G_CALLBACK(ags_add_sheet_page_dialog_response_callback), add_sheet_page_dialog);
}

void
ags_add_sheet_page_dialog_disconnect(AgsConnectable *connectable)
{
  AgsAddSheetPageDialog *add_sheet_page_dialog;

  add_sheet_page_dialog = AGS_ADD_SHEET_PAGE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (add_sheet_page_dialog->connectable_flags)) == 0){
    return;
  }

  add_sheet_page_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(add_sheet_page_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_add_sheet_page_dialog_response_callback),
		      add_sheet_page_dialog,
		      NULL);
}

void
ags_add_sheet_page_dialog_finalize(GObject *gobject)
{
  AgsAddSheetPageDialog *add_sheet_page_dialog;

  add_sheet_page_dialog = (AgsAddSheetPageDialog *) gobject;
  
  G_OBJECT_CLASS(ags_add_sheet_page_dialog_parent_class)->finalize(gobject);
}

void
ags_add_sheet_page_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_add_sheet_page_dialog_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsAddSheetPageDialog *add_sheet_page_dialog;
  
  AgsApplicationContext *application_context;

  xmlNode *root_node;
  xmlNode *node;

  gchar *str;
  
  add_sheet_page_dialog = AGS_ADD_SHEET_PAGE_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;
    
  machine = composite_editor->selected_machine;

  /* editor model */
  node = xmlNewNode(NULL,
		    BAD_CAST "ags-sheet-edit-page");

  str = gtk_editable_get_text(GTK_EDITABLE(add_sheet_page_dialog->sheet_title));
  
  xmlNewProp(node,
	     "title",
	     str);
  
  str = g_strdup_printf("%d",
			gtk_spin_button_get_value_as_int(add_sheet_page_dialog->notation_x0));
  
  xmlNewProp(node,
	     "x0",
	     str);

  g_free(str);

  str = g_strdup_printf("%d",
			gtk_spin_button_get_value_as_int(add_sheet_page_dialog->notation_x1));
    
  xmlNewProp(node,
	     "x1",
	     str);

  g_free(str);
  
  ags_machine_add_editor_model(machine,
			       node);
}

void
ags_add_sheet_page_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_add_sheet_page_dialog_new:
 *
 * Create a new #AgsAddSheetPageDialog.
 *
 * Returns: a new #AgsAddSheetPageDialog
 *
 * Since: 3.18.0
 */
AgsAddSheetPageDialog*
ags_add_sheet_page_dialog_new()
{
  AgsAddSheetPageDialog *add_sheet_page_dialog;

  add_sheet_page_dialog = (AgsAddSheetPageDialog *) g_object_new(AGS_TYPE_ADD_SHEET_PAGE_DIALOG,
								 NULL);

  return(add_sheet_page_dialog);
}
