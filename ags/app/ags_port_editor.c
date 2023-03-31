/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_port_editor.h>
#include <ags/app/ags_port_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_port_editor_class_init(AgsPortEditorClass *port_editor);
void ags_port_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_port_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_port_editor_init(AgsPortEditor *port_editor);

void ags_port_editor_connect(AgsConnectable *connectable);
void ags_port_editor_disconnect(AgsConnectable *connectable);

void ags_port_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_port_editor_apply(AgsApplicable *applicable);
void ags_port_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_port_editor
 * @short_description: Edit ports.
 * @title: AgsPortEditor
 * @section_id:
 * @include: ags/app/ags_port_editor.h
 *
 * #AgsPortEditor is a composite widget to modify ports. A port editor 
 * should be packed by plugin browser implementation.
 */

static gpointer ags_port_editor_parent_class = NULL;

GType
ags_port_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_port_editor = 0;

    static const GTypeInfo ags_port_editor_info = {
      sizeof (AgsPortEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_port_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPortEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_port_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_port_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_port_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_port_editor = g_type_register_static(GTK_TYPE_GRID,
						  "AgsPortEditor", &ags_port_editor_info,
						  0);

    g_type_add_interface_static(ags_type_port_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_port_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_port_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_port_editor_class_init(AgsPortEditorClass *port_editor)
{
  ags_port_editor_parent_class = g_type_class_peek_parent(port_editor);
}

void
ags_port_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_port_editor_connect;
  connectable->disconnect = ags_port_editor_disconnect;
}

void
ags_port_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_port_editor_set_update;
  applicable->apply = ags_port_editor_apply;
  applicable->reset = ags_port_editor_reset;
}

void
ags_port_editor_init(AgsPortEditor *port_editor)
{
  GtkLabel *label;

  GtkCellRenderer *port_control_cell_renderer_text;
  GtkCellRenderer *port_control_orientation_cell_renderer_text;
  
  GtkListStore *list_store;

  GtkTreeIter iter;

  gchar *str;

  gtk_grid_set_column_spacing((GtkGrid *) port_editor,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing((GtkGrid *) port_editor,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  port_editor->flags = 0;
  port_editor->connectable_flags = 0;
  
  /* port name */
  str = g_strdup_printf("%s: ",
			i18n("Port name"));
  
  label = (GtkLabel *) gtk_label_new(str);
  
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  g_free(str);
  
  port_editor->port_name = (GtkLabel *) gtk_label_new(NULL);  
  gtk_widget_set_halign((GtkWidget *) port_editor->port_name,
			GTK_ALIGN_START);
  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) port_editor->port_name,
		  1, 0,
		  1, 1);

  /* port control */
  str = g_strdup_printf("%s: ",
			i18n("Port control"));
  
  label = (GtkLabel *) gtk_label_new(str);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  g_free(str);
  
  port_editor->port_control = (GtkComboBox *) gtk_combo_box_new();
  
  port_control_cell_renderer_text = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(port_editor->port_control),
			     port_control_cell_renderer_text,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(port_editor->port_control),
				 port_control_cell_renderer_text,
				 "text", 0,
				 NULL);

  list_store = gtk_list_store_new(2,
				  G_TYPE_STRING,
				  G_TYPE_GTYPE);
  
  gtk_combo_box_set_model(port_editor->port_control,
			  GTK_TREE_MODEL(list_store));
  
  gtk_widget_set_halign((GtkWidget *) port_editor->port_control,
			GTK_ALIGN_START);

  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) port_editor->port_control,
		  1, 1,
		  1, 1);  

  /* port control orientation */
  str = g_strdup_printf("%s: ",
			i18n("Port control orientation"));

  label = (GtkLabel *) gtk_label_new(str);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  g_free(str);
  
  port_editor->port_control_orientation = (GtkComboBox *) gtk_combo_box_new();
  
  port_control_orientation_cell_renderer_text = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(port_editor->port_control_orientation),
			     port_control_orientation_cell_renderer_text,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(port_editor->port_control_orientation),
				 port_control_orientation_cell_renderer_text,
				 "text", 0,
				 NULL);

  list_store = gtk_list_store_new(2,
				  G_TYPE_STRING,
				  G_TYPE_UINT);

  /* vertical */
  gtk_list_store_append(list_store,
			&iter);
  
  gtk_list_store_set(list_store,
		     &iter,
		     0, "vertical",
		     1, GTK_ORIENTATION_VERTICAL,
		     -1);

  /* horizontal */
  gtk_list_store_append(list_store,
			&iter);
  gtk_list_store_set(list_store,
		     &iter,
		     0, "horizontal",
		     1, GTK_ORIENTATION_HORIZONTAL,
		     -1);  
  
  gtk_combo_box_set_model(port_editor->port_control_orientation,
			  GTK_TREE_MODEL(list_store));

  gtk_combo_box_set_active(port_editor->port_control_orientation,
			   0);

  /* attach */
  gtk_widget_set_halign((GtkWidget *) port_editor->port_control_orientation,
			GTK_ALIGN_START);

  gtk_grid_attach((GtkGrid *) port_editor,
		  (GtkWidget *) port_editor->port_control_orientation,
		  1, 2,
		  1, 1);  
}

void
ags_port_editor_connect(AgsConnectable *connectable)
{
  AgsPortEditor *port_editor;

  port_editor = AGS_PORT_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (port_editor->connectable_flags)) != 0){
    return;
  }

  port_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_port_editor_disconnect(AgsConnectable *connectable)
{
  AgsPortEditor *port_editor;

  port_editor = AGS_PORT_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (port_editor->connectable_flags)) == 0){
    return;
  }

  port_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_port_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_port_editor_apply(AgsApplicable *applicable)
{
  AgsPortEditor *port_editor;

  port_editor = AGS_PORT_EDITOR(applicable);

  //TODO:JK: implement me
}

void
ags_port_editor_reset(AgsApplicable *applicable)
{
  AgsPortEditor *port_editor;

  port_editor = AGS_PORT_EDITOR(applicable);

  //TODO:JK: implement me
}

/**
 * ags_port_editor_test_flags:
 * @port_editor: the #AgsPortEditor
 * @flags: the flags
 * 
 * Test @flags is set of @port_editor.
 * 
 * Returns: %TRUE if flags is set, otherwise %FALSE
 * 
 * Since: 4.0.0
 */
gboolean
ags_port_editor_test_flags(AgsPortEditor *port_editor,
			   guint flags)
{
  gboolean retval;
  
  g_return_val_if_fail(AGS_IS_PORT_EDITOR(port_editor), FALSE);

  retval = (flags & (port_editor->flags)) ? TRUE: FALSE;
  
  return(retval);
}

void
ags_port_editor_fill_controls(AgsPortEditor *port_editor)
{
  GtkListStore *list_store;

  GtkTreeIter iter;

  list_store = GTK_LIST_STORE(gtk_combo_box_get_model(port_editor->port_control));

  //TODO:JK: implement more
  
  if((AGS_PORT_EDITOR_IS_OUTPUT & (port_editor->flags)) != 0){
    if((AGS_PORT_EDITOR_IS_BOOLEAN & (port_editor->flags)) != 0){
      /* led */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_LED,
			 1, AGS_TYPE_LED,
			 -1);

      gtk_combo_box_set_active(port_editor->port_control,
			       0);
    }else{
      /* indicator */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_INDICATOR,
			 1, AGS_TYPE_INDICATOR,
			 -1);

      gtk_combo_box_set_active(port_editor->port_control,
			       0);
    }
  }else{
    if((AGS_PORT_EDITOR_IS_BOOLEAN & (port_editor->flags)) != 0){
      /* toggle button */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_TOGGLE_BUTTON,
			 1, GTK_TYPE_TOGGLE_BUTTON,
			 -1);

      /* check button */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_CHECK_BUTTON,
			 1, GTK_TYPE_CHECK_BUTTON,
			 -1);

      gtk_combo_box_set_active(port_editor->port_control,
			       1);
    }else{
      /* spin button */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_SPIN_BUTTON,
			 1, GTK_TYPE_SPIN_BUTTON,
			 -1);

      /* scale */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_SCALE,
			 1, GTK_TYPE_SCALE,
			 -1);

      /* dial */
      gtk_list_store_append(list_store,
			    &iter);
  
      gtk_list_store_set(list_store,
			 &iter,
			 0, AGS_PORT_EDITOR_CONTROL_DIAL,
			 1, AGS_TYPE_DIAL,
			 -1);

      gtk_combo_box_set_active(port_editor->port_control,
			       2);
    }
  }
}

/**
 * ags_port_editor_set_flags:
 * @port_editor: the #AgsPortEditor
 * @flags: the flags
 * 
 * Set @flags of @port_editor.
 * 
 * Since: 4.0.0
 */
void
ags_port_editor_set_flags(AgsPortEditor *port_editor,
			  guint flags)
{
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  port_editor->flags |= flags;
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(port_editor->port_control)));

  ags_port_editor_fill_controls(port_editor);  
}

/**
 * ags_port_editor_unset_flags:
 * @port_editor: the #AgsPortEditor
 * @flags: the flags
 * 
 * Unset @flags of @port_editor.
 * 
 * Since: 4.0.0
 */
void
ags_port_editor_unset_flags(AgsPortEditor *port_editor,
			    guint flags)
{
  g_return_if_fail(AGS_IS_PORT_EDITOR(port_editor));

  port_editor->flags &= (~flags);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(port_editor->port_control)));

  ags_port_editor_fill_controls(port_editor);  
}

/**
 * ags_port_editor_new:
 *
 * Create a new instance of #AgsPortEditor
 *
 * Returns: the new #AgsPortEditor
 *
 * Since: 3.0.0
 */
AgsPortEditor*
ags_port_editor_new()
{
  AgsPortEditor *port_editor;

  port_editor = (AgsPortEditor *) g_object_new(AGS_TYPE_PORT_EDITOR,
					       NULL);

  return(port_editor);
}
