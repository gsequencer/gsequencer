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

#include <ags/app/ags_connection_editor_bulk.h>
#include <ags/app/ags_connection_editor_bulk_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_connection_editor_bulk_class_init(AgsConnectionEditorBulkClass *connection_editor_bulk);
void ags_connection_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_bulk_init(AgsConnectionEditorBulk *connection_editor_bulk);
void ags_connection_editor_bulk_dispose(GObject *gobject);

void ags_connection_editor_bulk_connect(AgsConnectable *connectable);
void ags_connection_editor_bulk_disconnect(AgsConnectable *connectable);

void ags_connection_editor_bulk_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_bulk_apply(AgsApplicable *applicable);
void ags_connection_editor_bulk_reset(AgsApplicable *applicable);

static gpointer ags_connection_editor_bulk_parent_class = NULL;

/**
 * SECTION:ags_connection_editor_bulk
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorBulk
 * @section_id:
 * @include: ags/app/ags_connection_editor_bulk.h
 *
 * #AgsConnectionEditorBulk is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor bulks.
 */

GType
ags_connection_editor_bulk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_bulk = 0;

    static const GTypeInfo ags_connection_editor_bulk_info = {
      sizeof (AgsConnectionEditorBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_bulk_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_bulk = g_type_register_static(GTK_TYPE_BOX,
							     "AgsConnectionEditorBulk", &ags_connection_editor_bulk_info,
							     0);

    g_type_add_interface_static(ags_type_connection_editor_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_bulk,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_bulk);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_bulk_class_init(AgsConnectionEditorBulkClass *connection_editor_bulk)
{
  GObjectClass *gobject;

  ags_connection_editor_bulk_parent_class = g_type_class_peek_parent(connection_editor_bulk);

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_bulk;

  gobject->dispose = ags_connection_editor_bulk_dispose;
}

void
ags_connection_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_bulk_connect;
  connectable->disconnect = ags_connection_editor_bulk_disconnect;
}

void
ags_connection_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_bulk_set_update;
  applicable->apply = ags_connection_editor_bulk_apply;
  applicable->reset = ags_connection_editor_bulk_reset;
}

void
ags_connection_editor_bulk_init(AgsConnectionEditorBulk *connection_editor_bulk)
{
  GtkGrid *grid;
  GtkLabel *label;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_bulk),
				 GTK_ORIENTATION_VERTICAL);
  
  connection_editor_bulk->connectable_flags = 0;

  grid = gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) connection_editor_bulk,
		 (GtkWidget *) grid);

  label = gtk_label_new(i18n("soundcard"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  connection_editor_bulk->soundcard = (GtkComboBox *) gtk_combo_box_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->soundcard,
		  1, 0,
		  1, 1);

  label = gtk_label_new(i18n("first line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  connection_editor_bulk->first_line = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
										      0.0,
										      1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->first_line,
		  1, 1,
		  1, 1);

  label = gtk_label_new(i18n("first soundcard line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  connection_editor_bulk->first_soundcard_line = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
											   0.0,
											   1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->first_soundcard_line,
		  1, 2,
		  1, 1);

  label = gtk_label_new(i18n("count"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  connection_editor_bulk->count = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
										 0.0,
										 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->count,
		  1, 3,
		  1, 1);

  connection_editor_bulk->remove_bulk = gtk_button_new_from_icon_name("list-remove-symbolic");
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->remove_bulk,
		  3, 3,
		  1, 1);
}

void
ags_connection_editor_bulk_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_connection_editor_bulk_parent_class)->dispose(gobject);
}

void
ags_connection_editor_bulk_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_bulk->connectable_flags)) != 0){
    return;
  }

  connection_editor_bulk->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(connection_editor_bulk->soundcard, "changed",
			 G_CALLBACK(ags_connection_editor_bulk_soundcard_callback), connection_editor_bulk);

  g_signal_connect(connection_editor_bulk->remove_bulk, "clicked",
		   G_CALLBACK(ags_connection_editor_bulk_remove_bulk_callback), connection_editor_bulk);
}

void
ags_connection_editor_bulk_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_bulk->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_bulk->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(connection_editor_bulk->soundcard,
		      "any_signal::changed",
		      G_CALLBACK(ags_connection_editor_bulk_soundcard_callback),
		      connection_editor_bulk,
		      NULL);

  g_object_disconnect(connection_editor_bulk->remove_bulk,
		      "any_signal::clicked",
		      G_CALLBACK(ags_connection_editor_bulk_remove_bulk_callback),
		      connection_editor_bulk,
		      NULL);
}

void
ags_connection_editor_bulk_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

void
ags_connection_editor_bulk_apply(AgsApplicable *applicable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

void
ags_connection_editor_bulk_reset(AgsApplicable *applicable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

/**
 * ags_connection_editor_bulk_to_xml_node:
 * @connection_editor_bulk: the #AgsConnectionEditorBulk
 * 
 * Serialize @connection_editor_bulk to #xmlNode-struct.
 * 
 * Returns: the serialized #xmlNode-struct
 * 
 * Since: 4.0.0
 */
xmlNode*
ags_connection_editor_bulk_to_xml_node(AgsConnectionEditorBulk *connection_editor_bulk)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(connection_editor_bulk), NULL);

  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

/**
 * ags_connection_editor_bulk_from_xml_node:
 * @connection_editor_bulk: the #AgsConnectionEditorBulk
 * @node: the #xmlNode-struct
 * 
 * Parse @node and apply to @connection_editor_bulk.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_bulk_from_xml_node(AgsConnectionEditorBulk *connection_editor_bulk,
					 xmlNode *node)
{
  xmlNode *child;

  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(connection_editor_bulk));
  g_return_if_fail(node != NULL);

  //TODO:JK: implement me
}

/**
 * ags_connection_editor_bulk_new:
 *
 * Creates an #AgsConnectionEditorBulk
 *
 * Returns: a new #AgsConnectionEditorBulk
 *
 * Since: 4.0.0
 */
AgsConnectionEditorBulk*
ags_connection_editor_bulk_new()
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = (AgsConnectionEditorBulk *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_BULK,
								    NULL);

  return(connection_editor_bulk);
}
